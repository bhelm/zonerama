/*
 * Login.h
 *
 *  Created on: Jun 9, 2009
 *      Author: root
 */

#ifndef ZFRAME_H_
#define ZFRAME_H_


struct zFrame {
	 int monitor;
     int frameId;
     int eventId;
     QDateTime timestamp;
     QDateTime eventStartTime;
};

#endif /* ZFRAME_H_ */
