/*
 * LinkedMonitor.cpp
 *
 *  Created on: May 3, 2011
 *      Author: Bernd Michael Helm <zonerama at helmundwalter.de>
 *      GNU GPL v3
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

	m_holdLastPictureForSeconds = 0;

	m_standByImagePath = "";

}

LinkedMonitor::~LinkedMonitor()
{
	// TODO Auto-generated destructor stub
}


void LinkedMonitor::paintLastFrame(QPainter* painter)
{
	if(!m_myImage.isNull())
		painter->drawImage(m_offsetHorizontal,m_offsetVertical,m_myImage);
}

void LinkedMonitor::paintFrame(QPainter* painter, zFrame frame)
{
	//we are requested to draw the same image again, no need to load it
	if(m_lastFrame.eventId == frame.eventId && m_lastFrame.frameId == frame.frameId)
	{
		painter->drawImage(m_offsetHorizontal,m_offsetVertical,m_myImage);
	}else
	{

		QString filename = this->frameToFilename(frame);

		m_myImage.load(filename);

		if(m_myImage.isNull())
		{
			qDebug() << "error loading image" << filename << "(" << frame.frameId <<")";
			return;
		}

		if(m_outputImageHeight>1 && m_outputImageWidth>1 && (m_myImage.size().width() != m_outputImageWidth || m_myImage.size().height() != m_outputImageHeight))
		{
			m_myImage = m_myImage.scaled(m_outputImageWidth,m_outputImageHeight);
		}

		painter->drawImage(m_offsetHorizontal,m_offsetVertical,m_myImage);

		this->deleteFrameFromHDD(filename);
	}
}


void LinkedMonitor::deleteFrameFromHDD(QString filename)
{
	if(m_deleteMergedFramesFromHDD)
	{
		QFile myfile(filename);
		myfile.remove();
	}
}

void LinkedMonitor::deleteFrameFromHDD(zFrame frame)
{
	if(m_deleteMergedFramesFromHDD)
	{
		QString filename = this->frameToFilename(frame);
		this->deleteFrameFromHDD(filename);
	}
}

void LinkedMonitor::paintStandByImage(QPainter* painter)
{

	if(!m_standBy.isNull())
		painter->drawImage(m_offsetHorizontal,m_offsetVertical,m_standBy);

}

void LinkedMonitor::setStandByImagePath(QString m_standByImagePath)
{

	this->m_standByImagePath = m_standByImagePath;
	m_standBy.load(m_standByImagePath);

	if(m_standBy.isNull())
	{
		qDebug() << "Linkedmonitor" << m_linkedMonitorId << ": Cannot load image" << m_standByImagePath << ". file path correct and image valid?";
	}

	if(m_outputImageHeight>1 && m_outputImageWidth>1 && (m_standBy.size().width() != m_outputImageWidth || m_standBy.size().height() != m_outputImageHeight))
	{
		m_standBy = m_standBy.scaled(m_outputImageWidth,m_outputImageHeight);
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




