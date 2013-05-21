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


#include "modul.h"
#include "../log.h"

#include <stdlib.h>
#include <stdio.h>

Lmodul *LcreateModul(){
	Lmodul *m = malloc(sizeof(Lmodul));
	if(m == NULL){
		lielas_log((unsigned char*)"Can't allocate memory for new modul", LOG_LEVEL_ERROR);
		return NULL;
	}
	m->id = 0;
	m->address[0] = 0;
	m->channels = 0;
	m->channel[0] = NULL;
	return m;
}

void LdeleteModul(Lmodul *m){
	int i;

	for( i = 0; i < m->channels; i++){
		LdeleteChannel(m->channel[i]);

	}
	free(m);
}

void LaddChannel(Lmodul *m, Lchannel *channel){
	if(m->channels == MAX_CHANNELS){
		lielas_log((unsigned char*)"Can't add channel, maximum number of channels reached", LOG_LEVEL_WARN);
		return;
	}
	m->channel[m->channels+1] = channel;
  m->channels += 1;
	if(m->channels <= MAX_CHANNELS){
		m->channel[m->channels+1] = NULL;
	}
}

void LremoveChannel(Lmodul *m, Lchannel *channel){
	Lchannel *c;
	int i;
	int cIndex;

	//search channel
	for( cIndex = 0; cIndex < MAX_CHANNELS && cIndex < m->channels; cIndex++){
		if(m->channel[cIndex] == channel){
			break;
		}
	}
	if(m->channel[cIndex] != channel){
		lielas_log((unsigned char*)"Can't remove channel, channel not found", LOG_LEVEL_WARN);
		return;
	}

	c = m->channel[cIndex];

	// order channels if not the last channel will be removed
	if(cIndex != (m->channels - 1)){
		for( i = cIndex; i < m->channels; i++){
			m->channel[i] = m->channel[i+1];
		}
	}

	//remove channel
	LdeleteChannel(c);
	m->channels -= 1;
}

