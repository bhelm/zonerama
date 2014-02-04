/*
 * MergedMonitor.cpp
 *
 *  Created on: May 2, 2011
 *      Author: Bernd Michael Helm <zonerama at helmundwalter.de>
 *      GNU GPL v3
 */

#include "MergedMonitor.h"

#include <QFile>

MergedMonitor::MergedMonitor(int id,QString eventsDir)
{
	m_monitorId = id;
	m_eventsDir = eventsDir;

	qDebug() << "MergedMonitor" << m_monitorId << "initialized";
	qDebug() << "MergedMonitorDir" << m_eventsDir << "initialized";
	m_maxFPS = 0;
	m_forceFPS = 0;

}

bool MergedMonitor::generateMergedScene()
{
	//database ok?
	if (!m_db.isOpen())
	{
		qDebug() << "database not open!";
		return false;
	}

	m_processingStartTime = QDateTime::currentDateTime();

	//linked monitors present?
	if (m_linkedMonitors.count() == 0)
	{
		qDebug() << "no linked monitors given";
		return false;
	}

	QSqlQuery frameQ(m_db);
	//build up linkedmonitors set
	QString monitorList;

	foreach(LinkedMonitor* monitor, m_linkedMonitors)
		{
			if (monitorList.length() != 0)
				monitorList.append(",?");
			else
				monitorList.append('?');
		}



	QString	query("SELECT "
						"GROUP_CONCAT(MonitorId),"
						"GROUP_CONCAT(FrameId),"
						"TimeStamp,"
						"GROUP_CONCAT(StartTime),"
						"GROUP_CONCAT(EventId) "
				 " FROM "
				 	 	 "Events, Frames "
				 " WHERE Events.Id = EventId "
				 	 	 "AND MonitorId in (%1) "
				 " GROUP BY TimeStamp ORDER BY TimeStamp limit %2");
	query = query.arg(monitorList).arg(this->m_maxLength);

	frameQ.prepare(query);
	qDebug() << "sorting monitors";
	
	QMap<int, QMap<int, zFrame>*> sortedMonitors;

	int count = 0;
	foreach(LinkedMonitor* monitor, m_linkedMonitors)
	{
		frameQ.bindValue(count, monitor->getLinkedMonitorId());

		//dual-use foreach, also declare sortedMonitor arrays here
		QMap<int, zFrame>* monitorMap = new QMap<int, zFrame> ();
		sortedMonitors.insert(monitor->getLinkedMonitorId(), monitorMap);

		count++;
		//qDebug() << monitor;
	}

	if (!frameQ.exec())
	{
		qDebug() << "error executing frame query" << query
				<< m_db.lastError().text();
		return false;
	}

	QDateTime currentTime = QDateTime::currentDateTime();
	QDateTime lastTime = currentTime;
	QMap<int, zFrame> lastSecondFrame;

	QSqlQuery frameD(m_db);
	frameD.prepare("DELETE FROM Frames WHERE EventId = ? AND FrameId = ?");
	count = 0;
	qDebug() << "processing frames";
	while (frameQ.next())
	{
		QDateTime timestamp = frameQ.value(2).toDateTime();

		if(count<10 && timestamp.secsTo(currentTime) < (this->m_maxLength+this->m_maxTimeBetweenEvents))
		{
			qDebug() << "oldest event is younger than maxLength+maxTimeBetweenEvents, exiting";
			return false;
		}

		//if there is a huge gap between events, stop processing and leave it for the next run
		if(lastTime.secsTo(timestamp) > m_maxTimeBetweenEvents)
		{
			qDebug() << "maxTimeBetweenEvents reached, quit after" << count << "seconds of video.";
			break;
		}


		lastTime = timestamp;

		QStringList monitorIds = frameQ.value(0).toString().split(",");
		QStringList frameIds = frameQ.value(1).toString().split(",");
		QStringList eventStartTimesString = frameQ.value(3).toString().split(",");
		QStringList eventIds = frameQ.value(4).toString().split(",");
		QList<QDateTime> eventStartTimes;
		foreach(QString eventTime, eventStartTimesString)
		{
			//qDebug() << eventTime << QDateTime::fromString(eventTime,"yyyy-MM-dd hh:mm:ss");
			//qDebug() << eventTime << i << QDateTime::fromString(eventTime,"yyyy-MM-dd hh:mm:ss");
			eventStartTimes.append(QDateTime::fromString(eventTime,"yyyy-MM-dd hh:mm:ss"));
		}
		// monitorid, frameid

		QHash<int,int> eventLock;
		/*
		 * it sometimes happens that you have two events from the same camera overlapping,
		 * so you got the same frames for one second twice. if we render this, you would see one second repeatet.
		 * to avoid this, i created the eventLock, that records the eventid of the first frame per monitor.
		 * if there is another frame in the same second and same monitor, we just ignore it.
		 */


		//create zFrames and sort them to monitor-arrays
		for (int i = 0; i < monitorIds.count(); i++)
		{
			//qDebug() << "monitor" << i << "event" << eventIds.at(i);

			zFrame nframe;
			nframe.frameId = frameIds.at(i).toInt();
			nframe.eventId = eventIds.at(i).toInt();
			nframe.monitor = monitorIds.at(i).toInt();
			nframe.timestamp = timestamp;

			nframe.eventStartTime = eventStartTimes.at(i);

			/* if it is the first frame for the monitor, add the eventId to the eventLock */
			if(!eventLock.contains(nframe.monitor))
			{
				eventLock.insert(nframe.monitor,nframe.eventId);
			}

			/* if the stored eventId does not match the eventid of the current frame, ignore and delete it-*/
			if(eventLock.value(nframe.monitor) != nframe.eventId)
			{
				//oh my god, its late
				LinkedMonitor* evilMonitorIHateYou = m_linkedMonitors.value(nframe.monitor);

				if(evilMonitorIHateYou->getDeleteMergedFramesFromDB())
				{
					//qDebug() << "Deleting frame" << frame.frameId << "from event" << frame.eventId;
					frameD.bindValue(0,nframe.eventId);
					frameD.bindValue(1,nframe.frameId);
					frameD.exec();
				}

				evilMonitorIHateYou->deleteFrameFromHDD(nframe);
				/* removed that because we have no a small function for it.
				 * if(evilMonitorIHateYou->getDeleteMergedFramesFromDB())
				{
					QFile file(evilMonitorIHateYou->frameToFilename(nframe));
					file.remove();
				}*/

				continue;
			}



			QMap<int, zFrame>* currentMonitor = sortedMonitors.value(
					nframe.monitor);
			currentMonitor->insert(nframe.frameId, nframe);

			//qDebug() << timestamp << monitorIds.at(i) << frameIds.at(i);
		}


		int maxFrames = -1;
		QHash<int, QMapIterator<int, zFrame>*> iters;
		//get max frames per monitor and initialize iterators
		foreach(LinkedMonitor* monitor, m_linkedMonitors)
		{
			QMap<int, zFrame>* currentMonitor = sortedMonitors.value(monitor->getLinkedMonitorId());

			if (currentMonitor->count() > maxFrames)
			{
				maxFrames = currentMonitor->count();
			}

			QMapIterator<int, zFrame>* miter =
					new QMapIterator<int, zFrame> (*currentMonitor);
			iters.insert(monitor->getLinkedMonitorId(), miter);
			/*foreach(zFrame frame, *currentMonitor)
					{
						qDebug() << frame.monitor << frame.frameId
								<< frame.timestamp;
					}*/
		}

		if(maxFrames == -1)
		{
			//oho, whats that?
			continue;
		}

		int sourceMaxFrames = maxFrames;

		if(m_maxFPS != 0 && maxFrames > m_maxFPS)
		{
			maxFrames = m_maxFPS;
		}

		if(m_forceFPS != 0)
		{
			maxFrames = m_forceFPS;
		}

		//Determine highest frame rate and compute intra-second time delta
		if (m_delta > (1.0 / (float) maxFrames))
		{
			m_delta = 1.0 / (float) maxFrames;
		}

		/*
		 * this loop syncronizes the frames for one second, i.e. if cam1 has 2 and cam2 has 4 frames:
		 * Cam	Frame
		 * ------
		 * 1	1
		 * 2	1
		 * ------
		 * 1	2
		 * 2	2
		 * ------
		 * 2	3 <-- oh no, no frames from cam1 left!
		 * ------
		 * 2	4
		 *
		 * this loop will make it to
		 *
		 * Cam	Frame
		 * 1	1 <-- frame one
		 * 2	1
		 * ------
		 * 1	1 <-- frame one repeated to fill frames evenly to make a smooth movie
		 * 2	2
		 * ------
		 * 1	2 <-- frame two
		 * 2	3
		 * ------
		 * 1	2 <-- also repeated here
		 * 2	4
		 *
		 */


		QList<zMergedFrame*> mFrames;
		for (int i = 0; i < maxFrames; i++)
		{
			zMergedFrame* mFrame = new zMergedFrame;
			mFrame->timestamp=timestamp;
			mFrames.append(mFrame);
		}

		foreach(LinkedMonitor* monitor, m_linkedMonitors)
		{
			int lastFrame=-1;
			//qDebug() << "processing Monitor" << monitor->getLinkedMonitorId();
			QMap<int, zFrame>* currentMonitor = sortedMonitors.value(
					monitor->getLinkedMonitorId());
			int frameCount = currentMonitor->count();
			if (frameCount == 0)
			{
				if(lastSecondFrame.contains(monitor->getLinkedMonitorId()))
				{
					zFrame lastFrame = lastSecondFrame.value(monitor->getLinkedMonitorId());

					if(lastFrame.timestamp.secsTo(timestamp) <= monitor->getHoldLastPictureForSeconds())
					{
						for (int i = 0; i < maxFrames; i++)
						{
							mFrames.at(i)->frames.append(lastFrame);
						}
					}
				}

				continue;
			}
			float frameStep = (float) frameCount / maxFrames;
			//qDebug() << "iters" << monitor->getLinkedMonitorId();

			if(!iters.contains(monitor->getLinkedMonitorId()))
			{
				qDebug() << "no iterator for " << monitor->getLinkedMonitorId();
			}
			QMapIterator<int, zFrame>* currentIterator = iters.value(
					monitor->getLinkedMonitorId());


			float virtualFrame = 0.0;
			for (int i = 0; i < maxFrames; i++)
			{
				if (currentIterator->hasNext() && lastFrame!=floor(virtualFrame))
				{
					currentIterator->next();
					lastFrame=floor(virtualFrame);
				}

				//zFrame test = currentIterator->value();
				//qDebug() << "calc: " << currentIterator->value().frameId << virtualFrame << frameStep;
				mFrames.at(i)->frames.append(currentIterator->value());


				//lastSecondFrame.remove(monitor->getLinkedMonitorId());
				lastSecondFrame.insert(monitor->getLinkedMonitorId(),currentIterator->value());

				virtualFrame += frameStep;
			}

			//if the framerate is limited (with maxFPS or forceFPS), this takes care of deleting the remaining, not-rendered images if the delete options are set.
			for(int i=maxFrames;i<sourceMaxFrames;i++)
			{
				if (currentIterator->hasNext())
				{
					currentIterator->next();
					zFrame dframe = currentIterator->value();
					if(monitor->getDeleteMergedFramesFromDB())
					{
						//qDebug() << "Deleting frame" << frame.frameId << "from event" << frame.eventId;
						frameD.bindValue(0,dframe.eventId);
						frameD.bindValue(1,dframe.frameId);
						frameD.exec();
					}

					monitor->deleteFrameFromHDD(dframe);
				}
			}

			//qDebug() << "Frame"<< currentIterator->value().monitor << currentIterator->key();

			//	zFrame currentFrame =  currentIterator->value();
		}




		m_mergedScene.append(mFrames);

		//clean up sortedmonitors for next second
		foreach(LinkedMonitor* monitor, m_linkedMonitors)
		{
			QMap<int, zFrame>* currentMonitor = sortedMonitors.value(
					monitor->getLinkedMonitorId());
			currentMonitor->clear();
		}
		//qDebug() << "count: " << count;
		count++;
	}

	//cleanup
	foreach(LinkedMonitor* monitor, m_linkedMonitors)
	{
		delete sortedMonitors.value(monitor->getLinkedMonitorId());
	}

	return true;
}




void MergedMonitor::createEventSymlink(int eventId, zMergedFrame *firstFrame)
{
    QDir linkDir(this->m_eventsDir);
    linkDir.cd(QString("%1").arg(this->m_monitorId));
    
    QString datePath = firstFrame->timestamp.toString("yy/MM/dd");
    QString linkTarget = firstFrame->timestamp.toString("hh/mm/ss");
    linkDir.cd(datePath);
    
    QString linkPath = linkDir.absoluteFilePath(QString(".%1").arg(eventId));
    qDebug() << "Going to create symlink" << linkPath << "pointing to" << linkTarget;
    
    QFile::link(linkTarget, linkPath);
}

QString MergedMonitor::createEventDir(zMergedFrame* firstFrame)
{
	QString filename = this->m_eventsDir;
	filename.append(QString("/%1/").arg(this->m_monitorId));
	filename.append(firstFrame->timestamp.toString("yy/MM/dd/hh/mm/ss/"));

	QDir monitorDir(filename);
	if(!monitorDir.exists(filename))
	{
		if(!monitorDir.mkpath((filename)))
		{
			qDebug() << "cannot create Monitordir" << filename << ". Does the user that runs Zonerama have permission to create that directory? Try to run Zonerama as Root if unsure. -> exit";
			exit(-1);
		}
	}
	return filename;
}


int MergedMonitor::createEvent(zMergedFrame* firstFrame,zMergedFrame* lastFrame, int frameCount)
{
	m_mergedEventStartTime = firstFrame->timestamp;
	m_mergedEventEndTime = lastFrame->timestamp;

	QSqlQuery eventI(m_db);
	QString	query("INSERT INTO Events set MonitorId = ?, Name = ?, Cause = ?, StartTime = ?, EndTime = ?, Width = ?, Height = ?, Length = ?, Frames = ?");
	eventI.prepare(query);

	eventI.bindValue(0, m_monitorId);
	eventI.bindValue(1, m_name);
	eventI.bindValue(2, "Merge");
	eventI.bindValue(3, firstFrame->timestamp);
	eventI.bindValue(4, lastFrame->timestamp);
	eventI.bindValue(5, m_outputImageWidth);
	eventI.bindValue(6, m_outputImageHeight);
	eventI.bindValue(7, firstFrame->timestamp.secsTo(lastFrame->timestamp));
	eventI.bindValue(8, frameCount-1); //zm seem to use this not as count, but as "number of the last frame".
									   //frame counting starts at zero not at one, so we need to substract one frame
	if(eventI.exec())
	{
		int id = eventI.lastInsertId().toInt();
		eventI.clear();
		eventI.prepare("UPDATE Events set Name = CONCAT(Name,'-',?) WHERE Id = ?");
		eventI.bindValue(0,id);
		eventI.bindValue(1,id);
		eventI.exec();
		return id;
	}else
	{
		return 0;
	}
}

bool MergedMonitor::renderMergedScene()
{ 
	float frameDelta = 0;

	if(m_mergedScene.count()==0)
	{
		qDebug() << "mergedScene is empty!";
		return false;
	}

	qDebug() << "processing mergedScene with "<< m_mergedScene.count() << "frames";

	m_mergedEventDir = createEventDir(m_mergedScene.first());

	m_mergedEventId = createEvent(m_mergedScene.first(),m_mergedScene.last(),m_mergedScene.count());

	if(!m_deleteFramesAfterGenerateVideoFromHDD)
    		createEventSymlink(m_mergedEventId, m_mergedScene.first());

	QSqlQuery frameI(m_db);
	frameI.prepare(QString("INSERT INTO Frames SET EventId = '%1', FrameId = ?, TimeStamp = ?, Delta = ?").arg(m_mergedEventId));

	QSqlQuery frameD(m_db);
	frameD.prepare("DELETE FROM Frames WHERE EventId = ? AND FrameId = ?");


	//QHash<int,int> eventFrameCount;

	int count=0;


	foreach(zMergedFrame* mframe, m_mergedScene)
	{
		QImage output(m_outputImageWidth,m_outputImageHeight, QImage::Format_RGB32);
		output.fill(0);

		QPainter* painter = new QPainter(&output);

		//create a list with all monitor ids, so we know after rendering the frames which monitors got no image.
		QMap<int,int> monitorCheck;
		foreach(LinkedMonitor* lmoni, m_linkedMonitors)
		{
			monitorCheck.insert(lmoni->getLinkedMonitorId(),lmoni->getLinkedMonitorId());
		}

		//render the frames
		foreach(zFrame frame, mframe->frames)
		{
			m_linkedMonitors.value(frame.monitor)->paintFrame(painter,frame);
			monitorCheck.remove(frame.monitor);
		}

		foreach(int frameLessMonitorId, monitorCheck)
		{
			m_linkedMonitors.value(frameLessMonitorId)->paintStandByImage(painter);
		}




		QString fileName = m_mergedEventDir;
		fileName.append(QString("/%1-capture.jpg").arg(count,3,10,QLatin1Char('0')));
		if(output.save(fileName,"JPEG",70))
		{
			//if we would delete the inserted frames, just don't insert them. save the envoirement.
			if(!m_deleteFramesAfterGenerateVideoFromDB)
			{
				frameI.bindValue(0,count);
				frameI.bindValue(1,mframe->timestamp);
				frameI.bindValue(2,frameDelta);
				frameI.exec();
				frameDelta = frameDelta + m_delta;
			}

			foreach(zFrame frame, mframe->frames)
			{
				LinkedMonitor* moni = m_linkedMonitors.value(frame.monitor);
				if(moni->getDeleteMergedFramesFromDB())
				{
					frameD.bindValue(0,frame.eventId);
					frameD.bindValue(1,frame.frameId);
					if(!frameD.exec())
					{
						//qDebug() << "success, deleting file :)";
						qDebug() << "SQL ERROR error " << m_db.lastError().number()
														<< m_db.lastError().text();
					}
				}

				//qDebug() << "Deleting frame" << frame.frameId << "from event" << frame.eventId;


			}
		}
		delete mframe;
		delete painter;
		count++;
	}


	/*QSqlQuery eventU(m_db);
	eventU.prepare(QString("UPDATE Events SET Frames = Frames - ?, AlarmFrames = 0 WHERE EventId = ?"));*/

	int timediff = m_processingStartTime.secsTo(QDateTime::currentDateTime());
	if(timediff!=0)
	{
		qDebug() << "DONE rendering" << m_mergedScene.count() << "frames in" << timediff << "secs:" << (m_mergedScene.count()/timediff);
	}

	if(m_generateVideo)
	{
		this->generateVideo();
	}

	return true;
}


bool MergedMonitor::generateVideo()
{
	qDebug() << "Generating Video...";
	QString input = m_mergedEventDir;
	input.append("/%03d-capture.jpg");
	QString output = m_eventsDir;
	output.append(QString("/%1/").arg(this->m_monitorId));
	output.append(m_mergedEventStartTime.toString("yy-MM-dd-_-hh-mm-ss"));
	output.append(".avi");

	QString args = m_ffmpegArguments.arg(input).arg(output);
	qDebug() << "...using command" << m_ffmpegCommand << args;




	QString program = m_ffmpegCommand;
	QProcess *myProcess = new QProcess();
	myProcess->start(m_ffmpegCommand, args.split(" ",QString::SkipEmptyParts));
	myProcess->waitForFinished(999999999);
	//qDebug() << QString(myProcess->readAllStandardOutput());
	//qDebug() << QString(myProcess->readAllStandardError());

	if(m_deleteFramesAfterGenerateVideoFromHDD)
	{
		QDir evDir(m_mergedEventDir);
		QStringList images = evDir.entryList(QDir::Files);
		foreach(QString image, images)
		{
			evDir.remove(image);
		}
	}

	qDebug() << "done generating video";
	return true;
}


bool MergedMonitor::connectDB(Login l)
{

	if (l.database.length() == 0)
	{
		qDebug() << "zoneMinder configuration not loaded";
		return false;
	}

	m_db = QSqlDatabase::addDatabase(l.driver);
	m_db.setDatabaseName(l.database);
	m_db.setHostName(l.host);
	m_db.setUserName(l.user);
	m_db.setPassword(l.pass);

	if (!m_db.open())
	{
		qDebug() << "Unable to connect to database:" << m_db.lastError().text()
				<< "\n";
		return false;
	}
	qDebug() << "Database connected";



	QSqlQuery monitorQ(m_db);
	QString	query("SELECT Name FROM Monitors where Id = ?");
	monitorQ.prepare(query);

	monitorQ.bindValue(0, m_monitorId);
	monitorQ.exec();
	if(monitorQ.next())
	{
		m_name = monitorQ.value(0).toString();
	}else
	{
		qDebug() << "could not find a monitor for monitorId, exiting" << m_monitorId;
		exit(0);
	}

	return true;
}

int MergedMonitor::getMonitorId()
{
	return m_monitorId;
}

MergedMonitor::~MergedMonitor()
{
	foreach(LinkedMonitor* moni, m_linkedMonitors)
	{
		delete moni;
	}
}

void MergedMonitor::addLinkedMonitor(LinkedMonitor* moni)
{
	m_linkedMonitors.insert(moni->getLinkedMonitorId(),moni);
	qDebug() << "Monitor" <<  m_monitorId << ": added link to" << moni->getLinkedMonitorId();
}





