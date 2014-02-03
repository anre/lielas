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

#include "device.h"
#include "lwp.h"
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
  d->mac[0] = 0;
	d->mint[0] = 0;
	d->moduls = 0;
	d->modul[0] = 0;
  d->name[0] = 0;
  d->sw_ver[0] = 0;
  d->supply[0] = 0;
  d->datapakets = 0;

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
	device->modul[device->moduls+1] = modul;
  device->moduls+=1;
	if(device->moduls <= MAX_MODULS){
		device->modul[device->moduls+1] = NULL;
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

void LprintDeviceStructure(Ldevice *d, char *str, int size, int all){
  int pos = 0;
  int i, j;
  
  if(d == NULL){
    pos += snprintf(&str[pos], size - pos, "Device is null\n");
    return;
  }
  
  pos += snprintf(&str[pos], size - pos, "Device %s with id %u:\n", d->address, d->id);
  pos += snprintf(&str[pos], size - pos, "  name: %s\n", d->name);
  pos += snprintf(&str[pos], size - pos, "  software version: %s\n", d->sw_ver);
  pos += snprintf(&str[pos], size - pos, "  supply: %s\n", d->supply);
  pos += snprintf(&str[pos], size - pos, "  meassurement interval: %s\n", d->mint);
  for(i=0; i < MAX_MODULS; i++){
    if(d->modul[i] == NULL){
      if(all){ // print empty entries
          pos += snprintf(&str[pos], size - pos, "*************************************************\n");
          pos += snprintf(&str[pos], size - pos, "      modul %i is null\n", i);
      }
      break;
    }else{
      pos += snprintf(&str[pos], size - pos, "*************************************************\n");
      pos += snprintf(&str[pos], size - pos, "  Modul %i of %u with id %u and address %s:\n", i, d->moduls, d->modul[i]->id, d->modul[i]->address);
      pos += snprintf(&str[pos], size - pos, "    meassurement interval: %s\n", d->modul[i]->mint);
      for(j=0; j < MAX_CHANNELS; j++){
        if(d->modul[i]->channel[j] == NULL){
          if(all){ // print empty entries
            pos += snprintf(&str[pos], size - pos, "-------------------------------------------------\n");
            pos += snprintf(&str[pos], size - pos, "      channel %i is null\n", j);
          }
          break;
        }else{
          pos += snprintf(&str[pos], size - pos, "-------------------------------------------------\n");
          pos += snprintf(&str[pos], size - pos, "      Channel %i of %u with id %u and address %s:\n", 
                           j, d->modul[i]->channels, d->modul[i]->channel[j]->id, d->modul[i]->channel[j]->address);
          pos += snprintf(&str[pos], size - pos, "      type: %s\n", d->modul[i]->channel[j]->type);
          pos += snprintf(&str[pos], size - pos, "      class: %s\n", d->modul[i]->channel[j]->class);
          pos += snprintf(&str[pos], size - pos, "      unit: %s\n", d->modul[i]->channel[j]->unit);
        }
      }
    }
  }
}





