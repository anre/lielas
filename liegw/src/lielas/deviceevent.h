/*
*
*	Copyright (c) 2013 Andreas Reder
*	Author      : Andreas Reder <andreas@reder.eu>
*	File		: 
*
*	This File is part of lieweb, a web-gui for osd-compatible devices
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

#ifndef DEVICEEVENT_H
#define DEVICEEVENT_H

#include "lielas.h"
#include "device.h"

#define MAX_EVENTS		1000

#define EVENTTYPE_NO_EVENT			0
#define EVENTTYPE_READ_LOGGER		1
#define EVENTTYPE_CHANGE_SETTINGS	2
#define EVENTTYPE_GET_SETTINGS		3

typedef struct deviceevent_struct{
	Ldevice* d;
	int type;
	struct deviceevent_struct *nextDeviceEvent;
	struct deviceevent_struct *lastDeviceEvent;
} Levent;


#endif
