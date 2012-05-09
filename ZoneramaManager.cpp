/*
 * ZoneramaManager.cpp
 *
 *  Created on: May 2, 2011
 *      Author: Bernd Michael Helm <zonerama at helmundwalter.de>
 *      GNU GPL v3
 */

#include "ZoneramaManager.h"

ZoneramaManager::ZoneramaManager()
{
	qDebug() << "ZoneramaManager initialized";
	// TODO Auto-generated constructor stub
	this->readConfig();

	if (m_ZoneMinderConfigFilePath.length() == 0)
	{
		qDebug() << "config.xml not loaded, exit";
	}

	this->readZoneMinderConfig(m_ZoneMinderConfigFilePath);

	foreach(MergedMonitor* monitor, m_monitors)
	{
		qDebug() << m_zoneminderDbLogin.driver;
		monitor->connectDB(m_zoneminderDbLogin);
		if(monitor->generateMergedScene())
		{
			monitor->renderMergedScene();
		}
	}
}

ZoneramaManager::~ZoneramaManager()
{
	// TODO Auto-generated destructor stub
}

bool ZoneramaManager::readZoneMinderConfig(QString fileName)
{
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return false;

	while (!file.atEnd())
	{
		QByteArray line = file.readLine();
		line = line.trimmed();

		if (line.length() == 0)
			continue;
		if (line.startsWith('#'))
			continue;
		if (!line.contains('='))
			continue;

		int equalpos = line.indexOf('=');

		QString key = line.left(equalpos);
		QString value = line.right(line.length() - equalpos - 1);
		qDebug() << "got Config value: " << key << "=" << value;
		m_ZoneMinderConfig.insert(key, value);
	}

	m_zoneminderDbLogin.driver = "QMYSQL";
	m_zoneminderDbLogin.database = m_ZoneMinderConfig.value("ZM_DB_NAME");
	m_zoneminderDbLogin.user = m_ZoneMinderConfig.value("ZM_DB_USER");
	m_zoneminderDbLogin.pass = m_ZoneMinderConfig.value("ZM_DB_PASS");
	m_zoneminderDbLogin.host = m_ZoneMinderConfig.value("ZM_DB_HOST");


	return (m_ZoneMinderConfig.count() != 0); //true if count is not zero
}

bool ZoneramaManager::readConfig()
{
	//TODO: add proper error handling for missing values

	QDomDocument doc("zonerama");
	QFile file("zonerama.conf");
	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "could not open config.xml";
		return false;
	}

	QScriptEngine engine;
	QScriptValue config = engine.evaluate("(" + QString(file.readAll()) + ")");
	if (engine.hasUncaughtException())
	{
	        int line = engine.uncaughtExceptionLineNumber();
	        qDebug() << "uncaught exception at line" << line << ":";
	        qDebug() << engine.uncaughtExceptionBacktrace().join("\n");
	        return false;
	}

	m_ZoneMinderConfigFilePath = config.property("zoneMinderConfigFile").toString();
	m_ZoneMinderEventsDirPath = config.property("zoneMinderEventsDir").toString();


	QScriptValue monitors = config.property("mergedMonitors");

	for(int i=0;i<monitors.property("length").toInteger();i++)
	{
		QScriptValue monitor =  monitors.property(i);
		int monitorId = monitor.property("monitorId").toInt32();
		MergedMonitor* myMonitor = new MergedMonitor(monitorId,m_ZoneMinderEventsDirPath);
		m_monitors.insert(monitorId, myMonitor);
		myMonitor->setOutputImageHeight(monitor.property("outputImageHeight").toInt32());
		myMonitor->setOutputImageWidth(monitor.property("outputImageWidth").toInt32());
		myMonitor->setMaxLength(monitor.property("maxLength").toInt32());
		myMonitor->setMaxTimeBetweenEvents(monitor.property("maxTimeBetweenEvents").toInt32());
		myMonitor->setGenerateVideo(monitor.property("generateVideo").toBool());
		myMonitor->setFfmpegCommand(monitor.property("ffmpegCommand").toString());
		myMonitor->setFfmpegArguments(monitor.property("ffmpegArguments").toString());
		myMonitor->setDeleteFramesAfterGenerateVideoFromDB(monitor.property("deleteFramesAfterGenerateVideoFromDB").toBool());
		myMonitor->setDeleteFramesAfterGenerateVideoFromHDD(monitor.property("deleteFramesAfterGenerateVideoFromHDD").toBool());

		if(monitor.property("maxFPS").isValid()) myMonitor->setMaxFPS(monitor.property("maxFPS").toInt32());
		if(monitor.property("forceFPS").isValid()) myMonitor->setForceFPS(monitor.property("forceFPS").toInt32());

		//parse linkedmonitors
		QScriptValue linkedMonitors = monitor.property("linkedMonitors");
		for(int l=0;l<linkedMonitors.property("length").toInteger();l++)
		{
			QScriptValue linkedMonitor = linkedMonitors.property(l);
			int linkedMonitorId = linkedMonitor.property("linkedMonitorId").toInt32();
			if(linkedMonitorId>0)
			{
				LinkedMonitor* moni = new LinkedMonitor(linkedMonitorId, m_ZoneMinderEventsDirPath);
				myMonitor->addLinkedMonitor(moni);
				moni->setOffsetVertical(linkedMonitor.property("offsetVertical").toInt32());
				moni->setOffsetHorizontal(linkedMonitor.property("offsetHorizontal").toInt32());

				moni->setOutputImageHeight(linkedMonitor.property("outputImageHeight").toInt32());
				moni->setOutputImageWidth(linkedMonitor.property("outputImageWidth").toInt32());
				moni->setDeleteMergedFramesFromDB(linkedMonitor.property("deleteMergedFramesFromDB").toBool());
				moni->setDeleteMergedFramesFromHDD(linkedMonitor.property("deleteMergedFramesFromHDD").toBool());

				if(linkedMonitor.property("holdLastPictureForSeconds").isValid())
					moni->setHoldLastPictureForSeconds(linkedMonitor.property("holdLastPictureForSeconds").toInt32());
				if(linkedMonitor.property("standByImagePath").isValid())
					moni->setStandByImagePath(linkedMonitor.property("standByImagePath").toString());
			}


		}

	}



//	qDebug() <<monitors;

	/*if (!doc.setContent(&file))
	{
		qDebug() << "error parsing config.xml";
		file.close();
		return false;
	}
	file.close();
	QDomElement docElem = doc.documentElement();
	QDomNodeList nodeList = docElem.elementsByTagName("zoneMinderConfigFile");
	if (nodeList.count() > 0)
	{
		m_ZoneMinderConfigFilePath
				= nodeList.at(0).childNodes().at(0).nodeValue();
	}

	nodeList = docElem.elementsByTagName("monitor");

	for (int iDx = 0; iDx < nodeList.count(); iDx++)
	{
		QDomNode node = nodeList.at(iDx);
		int monitorId =  node.toElement().elementsByTagName("monitorId").at(0).childNodes().at(0).nodeValue().toInt();

		MergedMonitor* myMonitor = new MergedMonitor(monitorId);
		m_monitors.insert(monitorId, myMonitor);
		QDomNodeList linkedMonitors =  node.toElement().elementsByTagName("linkedMonitor");
		for (int iDy = 0; iDy < linkedMonitors.count(); iDy++)
		{
			int linkedId = linkedMonitors.at(iDy).toElement().elementsByTagName("linkedMonitorId").at(0).childNodes().at(0).nodeValue().toInt();
			myMonitor->addLinkedMonitor(linkedId);
		}


	}*/


	return false;
}


