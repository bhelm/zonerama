/*
 * MergedMonitor.h
 *
 *  Created on: May 2, 2011
 *      Author: Bernd Michael Helm <zonerama at helmundwalter.de>
 *      GNU GPL v3
 */

#ifndef MERGEDMONITOR_H_
#define MERGEDMONITOR_H_
#include <QList>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlResult>
#include <QDateTime>
#include <QStringList>
#include "Login.h"
#include "zFrame.h"
#include "zMergedFrame.h"
#include "LinkedMonitor.h"
#include "math.h"
#include <QImage>
#include <QProcess>
#include <QDir>

class MergedMonitor
{
public:
	MergedMonitor(int id, QString eventsDir);
	virtual ~MergedMonitor();
	void addLinkedMonitor(LinkedMonitor* moni);
	int getMonitorId();
	bool connectDB(Login l);

	bool generateMergedScene();
	bool renderMergedScene();
	bool generateVideo();


private:
    int createEvent(zMergedFrame *firstFrame, zMergedFrame *lastFrame, int frameCount);
    QString createEventDir(zMergedFrame *firstFrame);
    /** Creates necessary symbolic link for the event */
    void createEventSymlink(int eventId, zMergedFrame *firstFrame);
private:
    QHash<int,LinkedMonitor*> m_linkedMonitors;
    int m_monitorId;
    QSqlDatabase m_db;
    QList<zMergedFrame*> m_mergedScene;

    int m_outputImageHeight;
    int m_outputImageWidth;
    QString m_eventsDir;
    QString m_name;
    int m_maxLength;
    int m_maxTimeBetweenEvents;
    bool m_generateVideo;
    bool m_deleteFramesAfterGenerateVideoFromDB;
    bool m_deleteFramesAfterGenerateVideoFromHDD;
    QString m_ffmpegCommand;
    QString m_ffmpegArguments;

    QString m_mergedEventDir;
    QDateTime m_mergedEventStartTime;
    QDateTime m_mergedEventEndTime;
    int m_mergedEventId;

    QDateTime m_processingStartTime;

    int m_maxFPS;
    int m_forceFPS;
    float m_delta;

public:

    int getForceFPS() const
    {
        return m_forceFPS;
    }

    int getMaxFPS() const
    {
        return m_maxFPS;
    }

    void setForceFPS(int m_forceFPS)
    {
        this->m_forceFPS = m_forceFPS;
    }

    void setMaxFPS(int m_maxFPS)
    {
        this->m_maxFPS = m_maxFPS;
    }


    QHash<int, LinkedMonitor*> getLinkedMonitors() const
    {
        return m_linkedMonitors;
    }

    QList<zMergedFrame*> getMergedScene() const
    {
        return m_mergedScene;
    }

    int getOutputImageHeight() const
    {
        return m_outputImageHeight;
    }

    int getOutputImageWidth() const
    {
        return m_outputImageWidth;
    }


    int getMaxLength() const
    {
    	return m_maxLength;
    }

    int getMaxTimeBetweenEvents() const
    {
       	return m_maxTimeBetweenEvents;
    }

    void setMaxTimeBetweenEvents(int time)
    {
    	this->m_maxTimeBetweenEvents = time;
    }

    void setMaxLength(int length)
    {
     	this->m_maxLength = length;
    }

    void setOutputImageHeight(int m_outputImageHeight)
    {
        this->m_outputImageHeight = m_outputImageHeight;
    }

    void setOutputImageWidth(int m_outputImageWidth)
    {
        this->m_outputImageWidth = m_outputImageWidth;
    }

    bool getDeleteFramesAfterGenerateVideoFromDB() const
    {
        return m_deleteFramesAfterGenerateVideoFromDB;
    }

    bool getDeleteFramesAfterGenerateVideoFromHDD() const
    {
        return m_deleteFramesAfterGenerateVideoFromHDD;
    }

    QString getFfmpegCommand() const
    {
        return m_ffmpegCommand;
    }

    QString getFfmpegArguments() const
    {
        return m_ffmpegArguments;
    }

    void setFfmpegArguments(QString m_ffmpegArguments)
    {
        this->m_ffmpegArguments = m_ffmpegArguments;
    }

    bool getGenerateVideo() const
    {
        return m_generateVideo;
    }

    void setDeleteFramesAfterGenerateVideoFromDB(bool m_deleteFramesAfterGenerateVideoFromDB)
    {
        this->m_deleteFramesAfterGenerateVideoFromDB = m_deleteFramesAfterGenerateVideoFromDB;
    }

    void setDeleteFramesAfterGenerateVideoFromHDD(bool m_deleteFramesAfterGenerateVideoFromHDD)
    {
        this->m_deleteFramesAfterGenerateVideoFromHDD = m_deleteFramesAfterGenerateVideoFromHDD;
    }

    void setFfmpegCommand(QString m_ffmpegCommand)
    {
        this->m_ffmpegCommand = m_ffmpegCommand;
    }

    void setGenerateVideo(bool m_generateVideo)
    {
        this->m_generateVideo = m_generateVideo;
    }

};

#endif /* MERGEDMONITOR_H_ */
