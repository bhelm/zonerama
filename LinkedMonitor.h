/*
 * LinkedMonitor.h
 *
 *  Created on: May 3, 2011
 *      Author: Bernd Michael Helm <zonerama at helmundwalter.de>
 *      GNU GPL v3
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
	void paintLastFrame(QPainter* painter);
	void paintStandByImage(QPainter* painter);
	bool deleteLastImageFile();

	QString frameToFilename(zFrame frame);

	void deleteFrameFromHDD(zFrame frame);
	void deleteFrameFromHDD(QString filename);


private:
	int m_linkedMonitorId;
	int m_offsetVertical;
	int m_offsetHorizontal;
	int m_outputImageHeight;
	int m_outputImageWidth;

	bool m_deleteMergedFramesFromDB;
	bool m_deleteMergedFramesFromHDD;

    int m_holdLastPictureForSeconds;

    QString m_standByImagePath;

	QString m_eventsDir;

	zFrame m_lastFrame;

	QImage m_myImage;
	QImage m_standBy;


//getters/setters
public:


    QString getStandByImagePath() const
    {
        return m_standByImagePath;
    }

    void setStandByImagePath(QString m_standByImagePath);


    int getHoldLastPictureForSeconds() const
    {
        return m_holdLastPictureForSeconds;
    }

    void setHoldLastPictureForSeconds(int m_holdLastPictureForSeconds)
    {
        this->m_holdLastPictureForSeconds = m_holdLastPictureForSeconds;
    }

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
