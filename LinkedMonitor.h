/*
 * LinkedMonitor.h
 *
 *  Created on: May 3, 2011
 *      Author: root
 */

#ifndef LINKEDMONITOR_H_
#define LINKEDMONITOR_H_


#include <QPainter>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QImage>
#include "zFrame.h"

class LinkedMonitor
{
public:
	LinkedMonitor(int id, QString eventsDir);
	virtual ~LinkedMonitor();

	void paintFrame(QPainter* painter, zFrame frame);
	bool deleteLastImageFile();

	QString frameToFilename(zFrame frame);


private:
	int m_linkedMonitorId;
	int m_offsetVertical;
	int m_offsetHorizontal;
	int m_outputImageHeight;
	int m_outputImageWidth;

	bool m_deleteMergedFramesFromDB;
	bool m_deleteMergedFramesFromHDD;

	QString m_eventsDir;

	zFrame m_lastFrame;

	QImage myImage;


//getters/setters
public:

    int getLinkedMonitorId() const
    {
        return m_linkedMonitorId;
    }

    int getOffsetHorizontal() const
    {
        return m_offsetHorizontal;
    }

    int getOffsetVertical() const
    {
        return m_offsetVertical;
    }

    int getOutputImageHeight() const
    {
        return m_outputImageHeight;
    }

    int getOutputImageWidth() const
    {
        return m_outputImageWidth;
    }

    void setOffsetHorizontal(int m_offsetHorizontal)
    {
        this->m_offsetHorizontal = m_offsetHorizontal;
    }

    void setOffsetVertical(int m_offsetVertical)
    {
        this->m_offsetVertical = m_offsetVertical;
    }

    void setOutputImageHeight(int m_outputImageHeight)
    {
        this->m_outputImageHeight = m_outputImageHeight;
    }

    void setOutputImageWidth(int m_outputImageWidth)
    {
        this->m_outputImageWidth = m_outputImageWidth;
    }

    bool getDeleteMergedFramesFromDB() const
    {
        return m_deleteMergedFramesFromDB;
    }

    bool getDeleteMergedFramesFromHDD() const
    {
        return m_deleteMergedFramesFromHDD;
    }

    void setDeleteMergedFramesFromDB(bool m_deleteMergedFramesFromDB)
    {
        this->m_deleteMergedFramesFromDB = m_deleteMergedFramesFromDB;
    }

    void setDeleteMergedFramesFromHDD(bool m_deleteMergedFramesFromHDD)
    {
        this->m_deleteMergedFramesFromHDD = m_deleteMergedFramesFromHDD;
    }
};

#endif /* LINKEDMONITOR_H_ */
