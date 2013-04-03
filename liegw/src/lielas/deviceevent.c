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

#include "deviceevent.h"

#include <stdio.h>
#include <stdlib.h>

Levent *eventlist;

void InitDeviceEvents(){
	eventlist = NULL;
}

Levent *LcreateEvent(Ldevice* d, int eventtype){
	int i;

	Levent* newEvent = malloc(sizeof(Levent));
	Levent* event = eventlist;

	if(newEvent == NULL)
		return NULL;

	newEvent->d = d;
	newEvent->type = eventtype;

	if(eventlist == NULL){
		newEvent->lastDeviceEvent = NULL;
		newEvent->nextDeviceEvent = NULL;
		event = newEvent;
	}else{
		for(i = 0; i < MAX_EVENTS; i++){
			if(event->nextDeviceEvent == NULL){
				event->nextDeviceEvent = newEvent;
				newEvent->lastDeviceEvent = event;
				break;
			}
			event = event->nextDeviceEvent;
		}
	}
	return newEvent;
}

void LremoveEvent(Levent* event){
	event->lastDeviceEvent->nextDeviceEvent = event->nextDeviceEvent;
	event->nextDeviceEvent->lastDeviceEvent = event->lastDeviceEvent;
	free(event);
}
