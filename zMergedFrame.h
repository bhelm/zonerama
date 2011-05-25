/*
 * zMergedFrame.h
 *
 *  Created on: Jun 9, 2009
 *      Author: Bernd Michael Helm <zonerama at helmundwalter.de>
 *      GNU GPL v3
 */

#ifndef ZMERGEDFRAME_H_
#define ZMERGEDFRAME_H_


struct zMergedFrame {
     QDateTime timestamp;
     QList<zFrame> frames;
};

#endif /* ZMERGEDFRAME_H_ */
