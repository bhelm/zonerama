/*
 * LinkedMonitor.cpp
 *
 *  Created on: May 3, 2011
 *      Author: root
 */

#include "LinkedMonitor.h"

LinkedMonitor::LinkedMonitor(int id, QString eventsDir)
{

	m_linkedMonitorId = id;
	m_eventsDir = eventsDir;
	// TODO Auto-generated constructor stub

	m_lastFrame.eventId = 0;
	m_lastFrame.frameId = 0;

	m_outputImageHeight = -1;
	m_outputImageWidth = -1;

}

LinkedMonitor::~LinkedMonitor()
{
	// TODO Auto-generated destructor stub
}


void LinkedMonitor::paintFrame(QPainter* painter, zFrame frame)
{
	//we are requested to draw the same image again, no need to load it
	if(m_lastFrame.eventId == frame.eventId && m_lastFrame.frameId == frame.frameId)
	{
		painter->drawImage(m_offsetHorizontal,m_offsetVertical,myImage);
	}else
	{

		QString filename = this->frameToFilename(frame);

		myImage.load(filename);

		if(myImage.isNull())
		{
			qDebug() << "error loading image" << filename << "(" << frame.frameId <<")";
			return;
		}

		if(m_outputImageHeight>1 && m_outputImageWidth>1 && (myImage.size().width() != m_outputImageWidth || myImage.size().height() != m_outputImageHeight))
		{
			myImage = myImage.scaled(m_outputImageWidth,m_outputImageHeight);
		}

		painter->drawImage(m_offsetHorizontal,m_offsetVertical,myImage);

		if(m_deleteMergedFramesFromHDD)
		{
			QFile myfile(filename);
			myfile.remove();
		}
	}
}

QString LinkedMonitor::frameToFilename(zFrame frame)
{
	QString filename = m_eventsDir;
	filename.append(QString("/%1/").arg(frame.monitor));
	filename.append(frame.eventStartTime.toString("yy/MM/dd/hh/mm/ss/"));
	filename.append(QString("%1").arg(frame.frameId,3,10,QLatin1Char('0')));
	filename.append("-capture.jpg");
	return filename;
}




