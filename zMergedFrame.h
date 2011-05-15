/*
 * Login.h
 *
 *  Created on: Jun 9, 2009
 *      Author: root
 */

#ifndef ZMERGEDFRAME_H_
#define ZMERGEDFRAME_H_


struct zMergedFrame {
     QDateTime timestamp;
     QList<zFrame> frames;
};

#endif /* ZMERGEDFRAME_H_ */
