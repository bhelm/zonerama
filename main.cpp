/*
 *      Author: Bernd Michael Helm <zonerama at helmundwalter.de>
 *      GNU GPL v3
 */

#include <QtCore>
#include <QCoreApplication>

#include "ZoneramaManager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
	qDebug() << "foo";
    ZoneramaManager* zm = new ZoneramaManager();


   // return a.exec();
    return 0;
}
