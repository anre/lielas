/*
*
*	Copyright (c) 2013 Andreas Reder
*	Author      : Andreas Reder <andreas.reder@lielas.org>
*	File		: 
*
*	This File is part of lielas, see www.lielas.org for more information.
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#ifndef DEVICE_H_
#define DEVICE_H_

#include <time.h>

#include "lielas.h"
#include "modul.h"
#include "lwp.h"

#define DEVICE_NO_EVENT		0
#define DEVICE_ALARM		  1
#define DEVICE_GET_DATA	  2
#define DEVICE_CHANGE	    3

#define DEVICE_NOT_REGISTERED	0
#define DEVICE_REGISTERED		  1 

#define DEVICE_NO_LOGGER	    0
#define DEVICE_HAS_LOGGER	    1

#define DEVICE_MAX_STR_LEN    64

typedef struct Ldevice_struct{
  unsigned int id;
  int registered;
  char address[IPV6_STR_LEN];
  char mac[MAC_STR_LEN];
  char mint[INT_STR_LEN]; 
  char name[DEVICE_MAX_STR_LEN];
  char sw_ver[DEVICE_MAX_STR_LEN];
  char supply[DEVICE_MAX_STR_LEN];
  int moduls;
  int datapakets;
  Lmodul *modul[MAX_MODULS];
  struct tm lastProcessed;
  int hasLogger;
}Ldevice;

Ldevice *LcreateDevice();
void LdeleteDevice(Ldevice *d);
void LaddModul(Ldevice *device, Lmodul *modul);
void LremoveModul(Ldevice *device, Lmodul *modul);
void LprintDeviceStructure(Ldevice *d, char *str, int size, int all);


#endif /* DEVICE_H_ */
