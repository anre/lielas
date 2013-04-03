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

#include "device.h"
#include "../log.h"

#include <stdio.h>
#include <stdlib.h>


Ldevice *LcreateDevice(){
	Ldevice *d = malloc(sizeof(Ldevice));
	if(d == NULL){
		lielas_log((unsigned char*)"Can't allocate memory for new device", LOG_LEVEL_ERROR);
		return NULL;
	}

	d->id = 0;
	d->registered = 0;
	d->address[0] = 0;
	d->mint[0] = 0;
	d->pint[0] = 0;
	d->aint[0] = 0;
	d->moduls = 0;
	d->modul[0] = 0;
	d->event = DEVICE_NO_EVENT;

	return d;
}

void LdeleteDevice(Ldevice *d){
	int i;

	for(i = 0; i < d->moduls; i++){
		LdeleteModul(d->modul[i]);
	}

	free(d);
	d = NULL;
}

void LaddModul(Ldevice *device, Lmodul *modul){
	if(device->moduls == MAX_MODULS){
		lielas_log((unsigned char*)"Can't add modul, maximum number of moduls reached", LOG_LEVEL_WARN);
		return;
	}
	device->modul[device->moduls++] = modul;
	if(device->moduls <= MAX_MODULS){
		device->modul[device->moduls] = NULL;
	}
}

void LremoveModul(Ldevice *device, Lmodul *modul){
	Lmodul *m;
	int i;
	int mIndex;

	//search modul
	for( mIndex = 0; mIndex < MAX_CHANNELS && mIndex < modul->channels; mIndex++){
		if(device->modul[mIndex] == modul){
			break;
		}
	}
	if(device->modul[mIndex] != modul){
		lielas_log((unsigned char*)"Can't remove modul, modul not found", LOG_LEVEL_WARN);
		return;
	}

	m = device->modul[mIndex];

	// order moduls if not the last modul will be removed
	if(mIndex != (device->moduls - 1)){
		for( i = mIndex; i < device->moduls; i++){
			device->modul[i] = device->modul[i+1];
		}
	}

	//remove channel
	LdeleteModul(m);
	device->moduls -= 1;
}





