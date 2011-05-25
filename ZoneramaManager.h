/*
 * ZoneramaManager.h
 *
 *  Created on: May 2, 2011
 *      Author: Bernd Michael Helm <zonerama at helmundwalter.de>
 *      GNU GPL v3
 */

#ifndef ZONERAMAMANAGER_H_
#define ZONERAMAMANAGER_H_
#include <QMap>
#include <QString>
#include <QFile>
#include <QDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QStringList>
#include <QDomDocument>
#include <QDomNode>
#include "MergedMonitor.h"
#include "Login.h"
#include <QScriptEngine>
#include <QScriptValue>

class ZoneramaManager
{
public:
	ZoneramaManager();
	virtual ~ZoneramaManager();

private:
	bool readZoneMinderConfig(QString fileName);
	bool readConfig();
	bool connectDB();

private:
	QMap<QString,QString> m_ZoneMinderConfig;
	QMap<int, MergedMonitor*> m_monitors;
	Login m_zoneminderDbLogin;

	QString m_ZoneMinderConfigFilePath;
	QString m_ZoneMinderEventsDirPath;

};

#endif /* ZONERAMAMANAGER_H_ */
