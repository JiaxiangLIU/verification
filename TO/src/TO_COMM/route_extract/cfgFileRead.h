/*
 * cfgFileRead.h
 *
 *  Created on: Jan 14, 2015
 *      Author: ryan
 */

#ifndef CFGFILEREAD_H_
#define CFGFILEREAD_H_

#include "dataStruct.h"

struct rt_extrRoadStationTime *extrRoadStationTime(int *findErr);
void delete(struct rt_extrRoadStationTime *pt);

#endif /* CFGFILEREAD_H_ */
