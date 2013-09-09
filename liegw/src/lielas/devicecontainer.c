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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include "devicecontainer.h"
#include "../log.h"
#include "../sql/sql.h"
#include "../settings.h"
#include "ldb.h"
#include "../jsmn/jsmn.h"
#include "../coap/libcoap/coap.h"
#include "../coap/mycoap.h"
#include "lwp.h"
#include "../devicehandler.h"


static struct Ldc_struct *deviceContainer = NULL;
static struct Ldc_struct *dcPtr;

void loadModuls(Ldevice *d, char *moduls);
void loadChannels(Lmodul *m, char *channels);

int ipv6ToMac(const char* ip, char *mac);


/********************************************************************************************************************************
 *
 * 			Device Container handling functions
 *
 *
 ********************************************************************************************************************************/

int LDCinit(){
	deviceContainer = malloc(sizeof(Ldc));
	if(deviceContainer == NULL){
		lielas_log((unsigned char*)"Couldn't allocate memory for deviceContainer", LOG_LEVEL_ERROR);
		return -1;
	}
	deviceContainer->d   = NULL;
	deviceContainer->ldc = NULL;
	deviceContainer->ndc = NULL;

	dcPtr = NULL;
	atexit(LDCdelete);
	return 0;
}

void LDCdelete(){
	if(deviceContainer != NULL){
		struct Ldc_struct *dc = deviceContainer->ndc;
		while(dc != NULL){
			LDCremove(dc->ldc->d);
			if(dc->ndc == NULL){
				LDCremove(dc->d);
				free(deviceContainer);
				return;
			}
			dc = dc->ndc;
		}
		free(deviceContainer);
	}
}

int LDCadd(Ldevice *d){
	struct Ldc_struct *newDc;
	struct Ldc_struct *dc;

	if(deviceContainer == NULL){
		lielas_log((unsigned char*)"DeviceContainer not initialized", LOG_LEVEL_ERROR);
		return -1;
	}else if(deviceContainer->ndc == NULL && deviceContainer->ldc == NULL){
		deviceContainer->d = d;
		deviceContainer->ldc = deviceContainer;
	}else if(deviceContainer->ndc == NULL ){
		newDc = malloc(sizeof(Ldc));
		if(newDc == NULL){
			lielas_log((unsigned char*)"Couldn't allocate memory for deviceContainer", LOG_LEVEL_WARN);
			return -1;
		}
		newDc->d = d;
		newDc->ldc = deviceContainer;
		newDc->ndc = NULL;
		deviceContainer->ndc = newDc;
	}else{
		dc = deviceContainer;
		while(dc->ndc != NULL){
			dc = dc->ndc;
		}
		newDc = malloc(sizeof(Ldc));
		if(newDc == NULL){
			lielas_log((unsigned char*)"Couldn't allocate memory for deviceContainer", LOG_LEVEL_ERROR);
			return -1;
		}
		newDc->d = d;
		newDc->ldc = dc;
		newDc->ndc = NULL;
		dc->ndc = newDc;
	}
	return 0;
}

int LDCremove(Ldevice *d){
	struct Ldc_struct *dc;

	if(deviceContainer == NULL){
			lielas_log((unsigned char*)"DeviceContainer not initialized", LOG_LEVEL_ERROR);
			return -1;
		}

	//search device
	dc = deviceContainer;
	while(dc->ndc != NULL && dc->d != d){
		dc = dc->ndc;
	}

	if(dc->ndc == NULL && dc->d != d){
		return LDC_DEVICE_NOT_FOUND;
	}

	//remove device
	if(dc == deviceContainer){	//first device
		dc = deviceContainer->ndc;
		free(deviceContainer);
		deviceContainer = dc;
		if(deviceContainer != NULL){
			deviceContainer->ldc = deviceContainer;
		}
	}else if(dc->ndc == NULL){			// last device
		dc->ldc->ndc = NULL;
		free(dc);
	}else{
		dc->ldc->ndc = dc->ndc;
		dc->ndc->ldc = dc->ldc;
		free(dc);
	}
	return 0;
}


/********************************************************************************************************************************
 *
 * 			Device Container lielas functions
 *
 *
 ********************************************************************************************************************************/

/********************************************************************************************************************************
 * 		LDCloadDevices(): loads devices from sql-database
 ********************************************************************************************************************************/

int LDCloadDevices(){
	PGresult *res;
	Ldevice *d;
	int rows;
	int i;
	char moduls[500];
	char msg[500];
  char wkc[CLIENT_BUFFER_LEN];
	char st[SQL_STATEMENT_BUF_SIZE];
  char log[LOG_BUF_LEN];

	//query devices table
  
	snprintf(st, SQL_STATEMENT_BUF_SIZE, "SELECT id, address, mac, mint, pint, aint, moduls, registered, wkc FROM %s.%s", LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICES);
	res = SQLexec(st);

	if(PQresultStatus(res) != PGRES_TUPLES_OK){
		lielas_log((unsigned char*)"Failed to get devices or no device registered", LOG_LEVEL_WARN);
		PQclear(res);
		return -1;
	}
	rows = PQntuples(res);
	for( i = 0; i < rows; i++){

		d = LcreateDevice();
		if( d == NULL ){
			lielas_log((unsigned char*)"Failed to allocate memory for device", LOG_LEVEL_WARN);
			return LDC_FAILED_TO_ALOCATE_MEMORY;
		}

    //load device settings
		d->id = atoi(PQgetvalue(res, i, 0));
		//d->registered = 1;
		strcpy(d->address, PQgetvalue(res,i, 1));
		strcpy(d->mac, PQgetvalue(res, i, 2));
		strcpy(d->mint, PQgetvalue(res, i, 3));
		strcpy(d->pint, PQgetvalue(res, i, 4));
		strcpy(d->aint, PQgetvalue(res, i, 5));
		strcpy(moduls,  PQgetvalue(res, i, 6));
    //load .well-known/core
		strcpy(wkc,  PQgetvalue(res, i, 8));
    
    //parse wkc
    if(lwp_parse_wkc(wkc, &d->wkc) != 0){
      lielas_log((unsigned char*)"failed to parse .well-known/core", LOG_LEVEL_WARN);
      return -1;
    }
    
    //load Moduls
		loadModuls(d, moduls);
    
    //add device to container
		LDCadd(d);
		sprintf(msg, "Device %s loaded", d->address);
		lielas_log((unsigned char*)msg, LOG_LEVEL_DEBUG);

	}
	PQclear(res);
  
  snprintf(log, LOG_BUF_LEN, "%i devices loaded", i);
  lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);
  
	return 0;
}

/********************************************************************************************************************************
 * 		LDCloadModuls: loads moduls from device d from database
 ********************************************************************************************************************************/
void loadModuls(Ldevice *d, char *moduls){
	PGresult *res;
	char channels[500];
	char mstr[10];
  char st[SQL_STATEMENT_BUF_SIZE];
	int i = 0;
	int j = 0;
	Lmodul *modul;



	while(moduls[i] != 0){
		if(moduls[i] != ';'){
			mstr[j++] = moduls[i++];
		}
		if(moduls[i] == ';' || moduls[i] == 0){

			//allocate memory for modul
			modul = LcreateModul();
			if( modul == NULL ){
				lielas_log((unsigned char*)"Failed to allocate memory for modul", LOG_LEVEL_WARN);
				return;
			}

			//load modul
			mstr[j] = 0;
		  snprintf(st, SQL_STATEMENT_BUF_SIZE,
		            "SELECT id, address, channels, mint FROM %s.%s WHERE id=%s",
		            LDB_TBL_SCHEMA, LDB_TBL_NAME_MODULS, mstr);

			res = SQLexec(st);
			if(PQresultStatus(res) != PGRES_TUPLES_OK){
				lielas_log((unsigned char*)"Failed to get modul or no modul registered", LOG_LEVEL_WARN);
				PQclear(res);
				return;
			}

			modul->id = atoi(PQgetvalue(res, 0, 0));
			strcpy(modul->address, PQgetvalue(res, 0, 1));
			strcpy(channels, PQgetvalue(res, 0, 2));
			strcpy(modul->mint, PQgetvalue(res, 0, 3));
			loadChannels(modul, channels);
			LaddModul(d, modul);

			PQclear(res);
			j = 0;
			if(moduls[i] != 0){
				i += 1;
			}
		}
	}

}

/********************************************************************************************************************************
 * 		LDCloadChannels: loads channels from modul m from database
 ********************************************************************************************************************************/

void loadChannels(Lmodul *m, char *channels){
	PGresult *res;
	Lchannel *channel;
	char cstr[10];
	int i = 0;
	int j = 0;
  char st[SQL_STATEMENT_BUF_SIZE];

	while(channels[i] != 0){
		if(channels[i] != ';'){
			cstr[j++] = channels[i++];
		}
		if(channels[i] == ';' || channels[i] == 0){

			//allocate memory for channel
			channel = LcreateChannel();
			if( channel == NULL ){
				lielas_log((unsigned char*)"Failed to allocate memory for channel", LOG_LEVEL_WARN);
				return;
			}

			//load channel
			cstr[j] = 0;
      snprintf(st, SQL_STATEMENT_BUF_SIZE,
                "SELECT id, address, type, unit, exponent FROM %s.%s WHERE id=%s",
                LDB_TBL_SCHEMA, LDB_TBL_NAME_CHANNELS, cstr);
			res = SQLexec(st);
			if(PQresultStatus(res) != PGRES_TUPLES_OK){
				lielas_log((unsigned char*)"Failed to get channel or no channel registered", LOG_LEVEL_WARN);
				PQclear(res);
				return;
			}

			channel->id = atoi(PQgetvalue(res, 0, 0));
			strcpy(channel->address, PQgetvalue(res, 0, 1));
			strcpy(channel->type, PQgetvalue(res, 0, 2));
			strcpy(channel->unit, PQgetvalue(res, 0, 3));
      channel->exponent = strtod(PQgetvalue(res, 0, 4), NULL);
      
			LaddChannel(m, channel);

			PQclear(res);
			j = 0;
			if(channels[i] != 0){
				i += 1;
			}
		}
	}
}

/********************************************************************************************************************************
 * 		getNewId(): returns a free id
 ********************************************************************************************************************************/

static int getNewId(int type){
	PGresult *res;
	int id = 0;
  char st[SQL_STATEMENT_BUF_SIZE];

	if(type == ID_TYPE_DEVICE){
    snprintf(st, SQL_STATEMENT_BUF_SIZE, "SELECT id FROM %s.%s ORDER BY id DESC LIMIT 1", LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICES);
	}else if(type == ID_TYPE_MODUL){
    snprintf(st, SQL_STATEMENT_BUF_SIZE, "SELECT id FROM %s.%s ORDER BY id DESC LIMIT 1", LDB_TBL_SCHEMA, LDB_TBL_NAME_MODULS);
	}else if(type == ID_TYPE_CHANNEL){
    snprintf(st, SQL_STATEMENT_BUF_SIZE, "SELECT id FROM %s.%s ORDER BY id DESC LIMIT 1", LDB_TBL_SCHEMA, LDB_TBL_NAME_CHANNELS);
	}else{
		return 0;
	}

	res = SQLexec(st);
	if(PQntuples(res) > 0){
		id = atoi(PQgetvalue(res, 0, 0)) + 1;
	}else{
		id = 1;
	}

	PQclear(res);

	return id;
}

Ldevice *loadDeviceById(unsigned int id){
	PGresult *res;
	char st[SQL_BUFFER_SIZE	];
	Ldevice *d;
	char moduls[CLIENT_BUFFER_LEN];

	d = LcreateDevice();

	if(d == NULL){
		return d;
	}

	snprintf(st, SQL_BUFFER_SIZE, "SELECT id, address, mac, mint, pint, aint, moduls, registered FROM %s.%s WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICES, id);
	res = SQLexec(st);
	if(PQresultStatus(res) != PGRES_TUPLES_OK){
		lielas_log((unsigned char*)"Failed to get device by id", LOG_LEVEL_WARN);
		PQclear(res);
		if(d != NULL){
			LdeleteDevice(d);
		}
		return 0;
	}

	if(PQntuples(res) < 1){
		return 0;
	}

	d->id = atoi(PQgetvalue(res, 0, 0));
	//d->registered = 1;
	strcpy(d->address, PQgetvalue(res,0, 1));
	strcpy(d->mac, PQgetvalue(res,0, 2));
	strcpy(d->mint, PQgetvalue(res, 0, 3));
	strcpy(d->pint, PQgetvalue(res, 0, 4));
	strcpy(d->aint, PQgetvalue(res, 0, 5));
	strcpy(moduls,  PQgetvalue(res, 0, 6));
	loadModuls(d, moduls);
	PQclear(res);

	return d;
}

/********************************************************************************************************************************
 * 		LDCsaveWKC: write .well-known/core to database
 ********************************************************************************************************************************/
int LDCsaveWKC(char *wkc, Ldevice *d){
	PGresult *res;
	char st[SQL_STATEMENT_BUF_SIZE];
 
  snprintf(st, SQL_STATEMENT_BUF_SIZE, "UPDATE %s.%s SET wkc='%s' WHERE id=%u", LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICES, wkc, d->id);
	res = SQLexec(st);
	if(!res){
		lielas_log((unsigned char*)"Error executing SQL statement", LOG_LEVEL_WARN);
		PQclear(res);
		return -1;
	}
	PQclear(res);
  return 0;
}

/********************************************************************************************************************************
 * 		LDCsaveNewDevice: write new device to database
 ********************************************************************************************************************************/

int LDCsaveNewDevice(Ldevice *d){
	PGresult *res;
	char st[SQL_BUFFER_SIZE];
  char log[LOG_BUF_LEN];
	int id;

	id = getNewId(ID_TYPE_DEVICE);
	if(id == 0){
		lielas_log((unsigned char*)"Can't get new device id", LOG_LEVEL_WARN);
		return -1;
	}

	snprintf(st, SQL_BUFFER_SIZE, "INSERT INTO %s.%s (id, address, mac, registered, name,  mint, pint, aint) VALUES (%d, '%s', '%s', '%s', '%s', '%s', '%s', '%s')",
	         LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICES, id, d->address, d->mac, "true", d->name, d->mint, d->pint, d->aint);

	res = SQLexec(st);

	PQclear(res);

  snprintf(log, LOG_BUF_LEN, "device %s saved \n", d->address);
  lielas_log((unsigned char*) log, LOG_LEVEL_DEBUG);
	d->id = id;

	return 0;
}

/********************************************************************************************************************************
 * 		LDCsaveUpdatedDevice: checks if something changed and saves changes, including moduls and devices
 ********************************************************************************************************************************/

int LDCsaveUpdatedDevice(Ldevice *d){
	PGresult *res;
	char st[SQL_BUFFER_SIZE	];
	char buf[CLIENT_BUFFER_LEN];
	Ldevice *oldDevice;
	Lchannel *c, *oldChannel;
	int i, j;

	//get device
	oldDevice = loadDeviceById(d->id);

	if(oldDevice == NULL){
		LDCsaveNewDevice(d);
	}

	oldDevice = loadDeviceById(d->id);

	if(oldDevice == NULL){
		return -1;
	}

	if(strcmp(d->address, oldDevice->address)){
		snprintf(st, SQL_BUFFER_SIZE, "UPDATE %s.%s SET address=\"%s\" WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICES , d->address, d->id);
		res = SQLexec(st);
		if(!res){
			lielas_log((unsigned char*)"Error executing SQL statement", LOG_LEVEL_WARN);
			PQclear(res);
			return -1;
		}
		PQclear(res);
	}
	if(strcmp(d->mac, oldDevice->mac)){
		snprintf(st, SQL_BUFFER_SIZE, "UPDATE %s.%s SET mac='%s' WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICES , d->mac, d->id);
		res = SQLexec(st);
		if(!res){
			lielas_log((unsigned char*)"Error executing SQL statement", LOG_LEVEL_WARN);
			PQclear(res);
			return -1;
		}
		PQclear(res);
	}
	if(strcmp(d->mint, oldDevice->mint)){
		snprintf(st, SQL_BUFFER_SIZE, "UPDATE %s.%s SET mint=\"%s\" WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICES , d->mint, d->id);
		res = SQLexec(st);
		if(!res){
			lielas_log((unsigned char*)"Error executing SQL statement", LOG_LEVEL_WARN);
			PQclear(res);
			return -1;
		}
		PQclear(res);
	}
	if(strcmp(d->pint, oldDevice->pint)){
		snprintf(st, SQL_BUFFER_SIZE, "UPDATE %s.%s SET pint=\"%s\" WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICES , d->pint, d->id);
		res = SQLexec(st);
		if(!res){
			lielas_log((unsigned char*)"Error executing SQL statement", LOG_LEVEL_WARN);
			PQclear(res);
			return -1;
		}
		PQclear(res);
	}
	if(strcmp(d->aint, oldDevice->aint)){
		snprintf(st, SQL_BUFFER_SIZE, "UPDATE %s.%s SET aint=\"%s\" WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICES , d->aint, d->id);
		res = SQLexec(st);
		if(!res){
			lielas_log((unsigned char*)"Error executing SQL statement", LOG_LEVEL_WARN);
			PQclear(res);
			return -1;
		}
		PQclear(res);
	}

	for(i = 1; i < MAX_MODULS && d->modul[i] != NULL; i++){
		if(d->modul[i] != NULL && oldDevice->modul[i] == NULL){	// new modul

			//save new modul
			d->modul[i]->id = getNewId(ID_TYPE_MODUL);
			if(d->modul[i]->id == 0){
				lielas_log((unsigned char*)"Can't get new modul id", LOG_LEVEL_WARN);
				return -1;
			}
			snprintf(st, SQL_BUFFER_SIZE, "INSERT INTO %s.%s (id, address, mint, pint, aint) VALUES (%d, '%s', '%s', '%s', '%s')",
			         LDB_TBL_SCHEMA, LDB_TBL_NAME_MODULS , d->modul[i]->id, d->modul[i]->address, d->modul[i]->mint,
			         d->modul[i]->pint, d->modul[i]->aint);

			res = SQLexec(st);
			if(!res){
				lielas_log((unsigned char*)"Error executing SQL statement", LOG_LEVEL_WARN);
				PQclear(res);
				return -1;
			}
			PQclear(res);

			//update device-modul entry
			snprintf(st, SQL_BUFFER_SIZE, "SELECT moduls FROM %s.%s WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICES , d->id);
			res = SQLexec(st);
			if(!res){
				lielas_log((unsigned char*)"Error executing SQL statement", LOG_LEVEL_WARN);
				PQclear(res);
				return -1;
			}
			strcpy(buf, PQgetvalue(res, 0,0));
			if(buf[0] == 0){
				snprintf(st, SQL_BUFFER_SIZE, "UPDATE %s.%s SET moduls='%d' WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICES , d->modul[i]->id, d->id);
			}else{
				snprintf(st, SQL_BUFFER_SIZE, "UPDATE %s.%s SET moduls='%s;%d' WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICES , PQgetvalue(res, 0,0), d->modul[i]->id, d->id);
			}
			PQclear(res);
			res = SQLexec(st);
			if(!res){
				lielas_log((unsigned char*)"Error executing SQL statement", LOG_LEVEL_WARN);
				PQclear(res);
				return -1;
			}
		}else{	//update old modul-entry
			if(strcmp(d->modul[i]->address, oldDevice->modul[i]->address)){
				snprintf(st, SQL_BUFFER_SIZE, "UPDATE %s.%s SET address=\"%s\" WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_MODULS , d->modul[i]->address, d->modul[i]->id);
				res = SQLexec(st);
				if(!res){
					lielas_log((unsigned char*)"Error executing SQL statement", LOG_LEVEL_WARN);
					PQclear(res);
					return -1;
				}
				PQclear(res);
			}
			if(strcmp(d->modul[i]->mint, oldDevice->modul[i]->mint)){
				snprintf(st, SQL_BUFFER_SIZE, "UPDATE %s.%s SET mint=\"%s\" WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_MODULS , d->modul[i]->mint, d->modul[i]->id);
				res = SQLexec(st);
				if(!res){
					lielas_log((unsigned char*)"Error executing SQL statement", LOG_LEVEL_WARN);
					PQclear(res);
					return -1;
				}
				PQclear(res);
			}
			if(strcmp(d->modul[i]->pint, oldDevice->modul[i]->pint)){
				snprintf(st, SQL_BUFFER_SIZE, "UPDATE %s.%s SET pint=\"%s\" WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_MODULS , d->modul[i]->pint, d->modul[i]->id);
				res = SQLexec(st);
				if(!res){
					lielas_log((unsigned char*)"Error executing SQL statement", LOG_LEVEL_WARN);
					PQclear(res);
					return -1;
				}
				PQclear(res);
			}
			if(strcmp(d->modul[i]->aint, oldDevice->modul[i]->aint)){
				snprintf(st, SQL_BUFFER_SIZE, "UPDATE %s.%s SET aint=\"%s\" WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_MODULS , d->modul[i]->aint, d->modul[i]->id);
				res = SQLexec(st);
				if(!res){
					lielas_log((unsigned char*)"Error executing SQL statement", LOG_LEVEL_WARN);
					PQclear(res);
					return -1;
				}
				PQclear(res);
			}
		}
    
    
		for(j = 1; j < MAX_CHANNELS && d->modul[i]->channel[j] != NULL; j++){
			c = d->modul[i]->channel[j];
			if(oldDevice->modul[i] == NULL){
				oldChannel = NULL;
			}else{
				oldChannel = oldDevice->modul[i]->channel[j];
			}
			if(c != NULL && oldChannel == NULL){
				//save new channel
				c->id = getNewId(ID_TYPE_CHANNEL);
				if(c->id == 0){
					lielas_log((unsigned char*)"Can't get new channel id", LOG_LEVEL_WARN);
					return -1;
				}
				snprintf(st, SQL_BUFFER_SIZE, "INSERT INTO channels (id, address, type, unit, class, exponent) VALUES (%d, '%s', '%s', '%s', '%s', '%.0f')",
						 c->id, c->address, c->type, c->unit, c->class, c->exponent);
				res = SQLexec(st);
				if(!res){
					lielas_log((unsigned char*)"Error executing SQL statement", LOG_LEVEL_WARN);
					PQclear(res);
					return -1;
				}
				PQclear(res);

				//update modul-channel entry
				snprintf(st, SQL_BUFFER_SIZE, "SELECT channels FROM %s.%s WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_MODULS , d->modul[i]->id);
				res = SQLexec(st);
				if(!res){
					lielas_log((unsigned char*)"Error executing SQL statement", LOG_LEVEL_WARN);
					PQclear(res);
					return -1;
				}
				strcpy(buf, PQgetvalue(res, 0,0));
				if(buf[0] == 0){
					snprintf(st, SQL_BUFFER_SIZE, "UPDATE %s.%s SET channels='%d' WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_MODULS , c->id, d->modul[i]->id);
				}else{
					snprintf(st, SQL_BUFFER_SIZE, "UPDATE %s.%s SET channels='%s;%d' WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_MODULS ,  PQgetvalue(res, 0,0), c->id, d->modul[i]->id);
				}
				PQclear(res);
				res = SQLexec(st);
				if(!res){
					lielas_log((unsigned char*)"Error executing SQL statement", LOG_LEVEL_WARN);
					PQclear(res);
					return -1;
				}
			}else{	//update channel
				if(strcmp(c->address, oldChannel->address)){
					snprintf(st, SQL_BUFFER_SIZE, "UPDATE %s.%s SET address=\"%s\" WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_CHANNELS ,  c->address, c->id);
					res = SQLexec(st);
					if(!res){
						lielas_log((unsigned char*)"Error executing SQL statement", LOG_LEVEL_WARN);
						PQclear(res);
						return -1;
					}
					PQclear(res);
				}
				if(strcmp(c->type, oldChannel->type)){
					snprintf(st, SQL_BUFFER_SIZE, "UPDATE %s.%s SET type=\"%s\" WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_CHANNELS ,  c->type, c->id);
					res = SQLexec(st);
					if(!res){
						lielas_log((unsigned char*)"Error executing SQL statement", LOG_LEVEL_WARN);
						PQclear(res);
						return -1;
					}
					PQclear(res);
				}
				if(strcmp(c->unit, oldChannel->unit)){
					snprintf(st, SQL_BUFFER_SIZE, "UPDATE %s.%s SET unit=\"%s\" WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_CHANNELS ,  c->unit, c->id);
					res = SQLexec(st);
					if(!res){
						lielas_log((unsigned char*)"Error executing SQL statement", LOG_LEVEL_WARN);
						PQclear(res);
						return -1;
					}
					PQclear(res);
				}
        if(strcmp(c->class, oldChannel->class)){
					snprintf(st, SQL_BUFFER_SIZE, "UPDATE %s.%s SET class=\"%s\" WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_CHANNELS ,  c->class, c->id);
					res = SQLexec(st);
					if(!res){
						lielas_log((unsigned char*)"Error executing SQL statement", LOG_LEVEL_WARN);
						PQclear(res);
						return -1;
					}
					PQclear(res);
				}
        if(c->exponent != oldChannel->exponent){
					snprintf(st, SQL_BUFFER_SIZE, "UPDATE %s.%s SET exponent=\"%.0f\" WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_CHANNELS ,  c->exponent, c->id);
					res = SQLexec(st);
					if(!res){
						lielas_log((unsigned char*)"Error executing SQL statement", LOG_LEVEL_WARN);
						PQclear(res);
						return -1;
					}
					PQclear(res);
				}
			}
		}
	}
	return 0;
}

/********************************************************************************************************************************
 * 		LDCgetDeviceByAddress: searchs in devicecontainer for device with address adr
 ********************************************************************************************************************************/

int LDCgetDeviceByAddress(const char* adr, Ldevice **d){
	static struct Ldc_struct *dc;

	if(deviceContainer == NULL){
		if(LDCinit() != 0){
			lielas_log((unsigned char*)"Can't search for device, devicecontainer not initialized", LOG_LEVEL_WARN);
			return -1;
		}
	}

	if(deviceContainer->d == NULL){	// no devices in devicelist
		*d = NULL;
		return 0;
	}

	dc = deviceContainer;
	while(strcmp(dc->d->address, adr) != 0){
		if(dc->ndc == NULL){
			*d = NULL;
			return 0;
		}
		dc = dc->ndc;
	}
	*d = dc->d;
	return 0;
}

/********************************************************************************************************************************
 * 		LDCgetDeviceById: searchs in devicecontainer for device with id
 ********************************************************************************************************************************/

int LDCgetDeviceById(int id, Ldevice **d){
	static struct Ldc_struct *dc;

	if(deviceContainer == NULL){
		if(LDCinit() != 0){
			lielas_log((unsigned char*)"Can't search for device, devicecontainer not initialized", LOG_LEVEL_WARN);
			return -1;
		}
	}

	if(deviceContainer->d == NULL){	// no devices in devicelist
		*d = NULL;
		return 0;
	}

	dc = deviceContainer;
	while(dc->d->id != id){
		if(dc->ndc == NULL){
			*d = NULL;
			return 0;
		}
		dc = dc->ndc;
	}
	*d = dc->d;
	return 0;
}


/********************************************************************************************************************************
 * 		LDCgetFirstDevice: returns first device in devicecontainer
 ********************************************************************************************************************************/

Ldevice *LDCgetFirstDevice(){
	if(deviceContainer == NULL){
		return NULL;
	}
	dcPtr = deviceContainer;
	return dcPtr->d;
}

/********************************************************************************************************************************
 * 		LDCgetNextDevice: returns the next device in devicecontainer
 ********************************************************************************************************************************/
Ldevice *LDCgetNextDevice(){
	if(dcPtr == NULL){
		return NULL;
	}
	dcPtr = dcPtr->ndc;
	if(dcPtr == NULL){
		return NULL;
	}
	return dcPtr->d;
}

/********************************************************************************************************************************
 * 		LDCcheckForNewDevices: checks gatewaynode for new devices
 ********************************************************************************************************************************/

void LDCcheckForNewDevices(){

	int i, j;
	int state = 0;
	char cmd[CLIENT_BUFFER_LEN];
	char buf[CLIENT_BUFFER_LEN];
	char adr[IPV6_ADR_BUF_LEN];
  char attr[CLIENT_BUFFER_LEN];
  char rplTable[CLIENT_BUFFER_LEN];
  char wkc[CLIENT_BUFFER_LEN];
  char mac[MAC_STR_LEN];
  char ex[CLIENT_BUFFER_LEN];
  unsigned char payload[CLIENT_BUFFER_LEN];
  char log[LOG_BUF_LEN];
	Ldevice *d;
	Lmodul *m[MAX_MODULS];
	Lchannel *c[MAX_CHANNELS];
	time_t rawtime;
	struct tm *now;
	static struct tm *nextScan;
	double diff;
  coap_buf *cb;
	jsmn_parser json;
	jsmnerr_t r;
	jsmntok_t tokens[MAX_JSON_TOKENS];
  int nextToken;
  int routes;
  int channels;
  int moduls;

	//get systemtime
	time(&rawtime);
	now = gmtime(&rawtime);

	if(nextScan == NULL){
		nextScan = malloc(sizeof(struct tm));
		if(nextScan == NULL)
			return;
		memcpy(nextScan, now, sizeof(struct tm));
	}else{
		diff = difftime(mktime(now), mktime(nextScan));
		if(diff < 1.0 && diff > (-MAX_SCAN_NEW_DEVICES_INTERVAL)){
			return;
		}
	}
	if(nextScan == NULL)
		return;

	free(nextScan);
	nextScan = malloc(sizeof(struct tm));
	if(nextScan == NULL)
		return;
	memcpy(nextScan, now, sizeof(struct tm));
	nextScan->tm_sec += SCAN_NEW_DEVICES_INTERVAL;
	mktime(nextScan);

	// get routing table
   
  cb = coap_create_buf();
	if( cb == NULL){
    lielas_log((unsigned char*)"failed to create coap buf", LOG_LEVEL_WARN);
		return;
	}
	cb->buf = (char*)rplTable;
  cb->bufSize = CLIENT_BUFFER_LEN;
  
  #ifdef DC_USE_RPL_COAP_SERVER
  snprintf(cmd, CLIENT_BUFFER_LEN, "coap://[%s]:%s/rpl", set_getGatewaynodeAddr(), set_getGatewaynodePort());
  coap_send_cmd(cmd, cb, MYCOAP_METHOD_GET, NULL);
  
  if(cb->status != COAP_STATUS_CONTENT){
    lielas_log((unsigned char*)"failed to get routing table", LOG_LEVEL_WARN);
    return;
  }
  
  #else 
  
  FILE *fp;
  long size;
  
  fp = fopen("/usr/local/lielas/rpl.tbl", "r");
  if(fp == NULL){
    lielas_log((unsigned char*)"failed to get routing table file", LOG_LEVEL_WARN);
    return;
  }
  
  //get file size
  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  
  if(cb->bufSize > (size + 1)){
    fread(cb->buf, size, 1, fp);
    cb->buf[size] = 0;
  }else{
    lielas_log((unsigned char*)"rpl routing table too big for buffer, skip parsing", LOG_LEVEL_WARN);
    fclose(fp);
    return;
  }
  
  fclose(fp);
  #endif
  jsmn_init(&json);
  nextToken = 0;
	r = jsmn_parse(&json, rplTable, tokens, MAX_JSON_TOKENS);
	if(r != JSMN_SUCCESS && r != JSMN_ERROR_PART){
    lielas_log((unsigned char*)"failed to parse routing table", LOG_LEVEL_WARN);
		return;
	}
  
  cb->buf = (char*)buf;
  cb->bufSize = CLIENT_BUFFER_LEN;
  
	while(nextToken < json.toknext && rplTable[tokens[nextToken].start] != 0){
		if(state == 0){ // "Routes" not yet found
      if( tokens[nextToken].type == JSMN_STRING){
        if(!strncmp(&rplTable[tokens[nextToken].start], "Routes", strlen("Routes"))){
          nextToken += 1;
          if(tokens[nextToken].type != JSMN_ARRAY){
            lielas_log((unsigned char*)"error parsing routing table", LOG_LEVEL_WARN);
            return;
          }
          routes = tokens[nextToken].size;
          
          if(routes == 0){
            lielas_log((unsigned char*)"routing table parsed, no routes found", LOG_LEVEL_DEBUG);
            return;
          }
          state += 1; 
          nextToken += 1;
        }
      } 
		}
		if(state == 1){	// "Routes" found, read and parse addresses
      if( tokens[nextToken].type == JSMN_STRING){
        //copy address to buffer
        for(i=0; i < tokens[nextToken].end - tokens[nextToken].start; i++){
          if(rplTable[i + tokens[nextToken].start] == ' '){
            adr[i]= 0;
            break;
          }
          adr[i] = rplTable[i + tokens[nextToken].start];
        }

        //check if address is already registered
        LDCgetDeviceByAddress(adr, &d);
				if(d == NULL){
          //new device found
          lielas_log((unsigned char*)"new device found", LOG_LEVEL_DEBUG);
					d = LcreateDevice();
					strcpy(d->address, adr);
					sprintf(d->mint, "%d", LIELAS_STD_MINT);
					sprintf(d->pint, "%d", LIELAS_STD_PINT);
					sprintf(d->aint, "%d", LIELAS_STD_AINT);
      
          //get wkc
          
          cb->buf = (char*)wkc;
          cb->bufSize = CLIENT_BUFFER_LEN;
          
          snprintf(cmd, CLIENT_BUFFER_LEN, "coap://[%s]:%s/.well-known/core", adr, set_getGatewaynodePort());
          coap_send_cmd(cmd, cb, MYCOAP_METHOD_GET, NULL);
          
          if(cb->status != COAP_STATUS_CONTENT){
            lielas_log((unsigned char*)"failed to get .well-known/core", LOG_LEVEL_WARN);
            return;
          }
          
          
          //parse wkc
          if(lwp_parse_wkc(cb->buf, &d->wkc) != 0){
            lielas_log((unsigned char*)"failed to parse .well-known/core", LOG_LEVEL_WARN);
            return;
          }
          
          //get resource info
          
          cb->buf = (char*)buf;
          cb->bufSize = CLIENT_BUFFER_LEN;
          
          snprintf(cmd, CLIENT_BUFFER_LEN, "coap://[%s]:%s/info", adr, set_getGatewaynodePort());
          coap_send_cmd(cmd, cb, MYCOAP_METHOD_GET, NULL);
          
          if(cb->status != COAP_STATUS_CONTENT){
            lielas_log((unsigned char*)"failed to get /info", LOG_LEVEL_WARN);
            return;
          }
          
          if(lwp_get_attr_value(cb->buf, &d->wkc.info, LWP_ATTR_INFO_NAME, d->name, DEVICE_MAX_STR_LEN)){
            lielas_log((unsigned char*)"failed to parse .well-known/core: /info/name", LOG_LEVEL_WARN);
            return;
          }
          if(lwp_get_attr_value(cb->buf, &d->wkc.info, LWP_ATTR_INFO_SW_VER, d->sw_ver, DEVICE_MAX_STR_LEN)){
            lielas_log((unsigned char*)"failed to parse .well-known/core: /info/sw_ver", LOG_LEVEL_WARN);
            return;
          }
          
          //get resource network
          snprintf(cmd, CLIENT_BUFFER_LEN, "coap://[%s]:%s/network", adr, set_getGatewaynodePort());
          coap_send_cmd(cmd, cb, MYCOAP_METHOD_GET, NULL);     
              
          if(cb->status != COAP_STATUS_CONTENT){
            lielas_log((unsigned char*)"failed to get /network", LOG_LEVEL_WARN);
            return;
          }
          
          if(lwp_get_attr_value(cb->buf, &d->wkc.network, LWP_ATTR_NETWORK_MAC, mac, MAC_STR_LEN)){
            lielas_log((unsigned char*)"failed to parse .well-known/core: /network/mac", LOG_LEVEL_WARN);
            return;
          }
          lwp_mac_to_std_mac(d->mac, mac);
          
          //get resource device
          snprintf(cmd, CLIENT_BUFFER_LEN, "coap://[%s]:%s/device", adr, set_getGatewaynodePort());
          coap_send_cmd(cmd, cb, MYCOAP_METHOD_GET, NULL);
          
          if(cb->status != COAP_STATUS_CONTENT){
            lielas_log((unsigned char*)"failed to get /device", LOG_LEVEL_WARN);
            return;
          }
          
          if(lwp_get_attr_value(cb->buf, &d->wkc.device, LWP_ATTR_DEVICE_SUPPLY, d->supply, DEVICE_MAX_STR_LEN)){
            lielas_log((unsigned char*)"failed to parse .well-known/core: /device/supply", LOG_LEVEL_WARN);
            return;
          }
          if(lwp_get_attr_value(cb->buf, &d->wkc.device, LWP_ATTR_DEVICE_MODULE_CNT, attr, CLIENT_BUFFER_LEN)){
            lielas_log((unsigned char*)"failed to parse .well-known/core: /device/module_cnt", LOG_LEVEL_WARN);
            return;
          }
          moduls = strtol(attr, NULL, 10);
          if(moduls > MAX_MODULS){
            lielas_log((unsigned char*)"failed to parse .well-known/core: /device/module_cnt, too big", LOG_LEVEL_WARN);
            return;
          }
          
          snprintf(log, LOG_BUF_LEN, "found Device with name %s and version %s is running on %s with %i modul(s)", d->name, d->sw_ver, d->supply, moduls);
          lielas_log((unsigned char*) log, LOG_LEVEL_DEBUG);
          
          //scan moduls
          for(i=1; i <= moduls; i++){
            //create modul
						m[i] = LcreateModul();
            sprintf(m[i]->address, "%i", i);
            sprintf(m[i]->mint, "%d", LIELAS_STD_MINT);
            sprintf(m[i]->pint, "%d", LIELAS_STD_PINT);
            sprintf(m[i]->aint, "%d", LIELAS_STD_AINT);
          
            //switch to modul
            if(i > 0){
              // TBD
            }
            
            // get modul
            snprintf(cmd, CLIENT_BUFFER_LEN, "coap://[%s]:%s/modul", adr, set_getGatewaynodePort());
            coap_send_cmd(cmd, cb, MYCOAP_METHOD_GET, NULL);
            
            if(lwp_get_attr_value(cb->buf, &d->wkc.modul, LWP_ATTR_MODUL_CHANNEL_CNT, attr, CLIENT_BUFFER_LEN)){
              lielas_log((unsigned char*)"failed to parse .well-known/core: /modul/channel_cnt", LOG_LEVEL_WARN);
              return;
            }
            channels = strtol(attr, NULL, 10);
            if(channels > MAX_CHANNELS){ 
              lielas_log((unsigned char*)"failed to parse .well-known/core: /device/channel_cnt, too big", LOG_LEVEL_WARN);
              return;
            }
						LaddModul(d, m[i]);
            snprintf(log, LOG_BUF_LEN, "found Modul %s with %i channels", m[i]->address, channels);
            lielas_log((unsigned char*) log, LOG_LEVEL_DEBUG);
            
            //scan channels
            for(j=1; j <= channels ; j++){
								c[j] = LcreateChannel();
                sprintf(c[j]->address, "%i", j);
                
                // get channel
                snprintf(cmd, CLIENT_BUFFER_LEN, "coap://[%s]:%s/channel%i", adr, set_getGatewaynodePort(), j);
                coap_send_cmd(cmd, cb, MYCOAP_METHOD_GET, NULL);
                
                if(cb->status != COAP_STATUS_CONTENT){
                  snprintf(log, LOG_BUF_LEN, "failed to get /channel%i", j);
                  lielas_log((unsigned char*)log, LOG_LEVEL_WARN);
                  return;
                }
                
                //unit
                if(lwp_get_attr_value(cb->buf, &d->wkc.channel[j], LWP_ATTR_CHANNEL_UNIT, c[j]->unit, CHANNEL_ATTR_STR_LEN)){
                  snprintf(log, LOG_BUF_LEN, "failed to parse .well-known/core: /channel%i/unit", j);
                  lielas_log((unsigned char*)log, LOG_LEVEL_WARN);
                  return;
                }
                
                //type
                if(lwp_get_attr_value(cb->buf, &d->wkc.channel[j], LWP_ATTR_CHANNEL_TYPE, c[j]->type, CHANNEL_ATTR_STR_LEN)){
                  snprintf(log, LOG_BUF_LEN, "failed to parse .well-known/core: /channel%i/type", j);
                  lielas_log((unsigned char*)log, LOG_LEVEL_WARN);
                  return;
                }   
                
                //class
                if(lwp_get_attr_value(cb->buf, &d->wkc.channel[j], LWP_ATTR_CHANNEL_CLASS, c[j]->class, CHANNEL_ATTR_STR_LEN)){
                  snprintf(log, LOG_BUF_LEN, "failed to parse .well-known/core: /channel%i/class", j);
                  lielas_log((unsigned char*)log, LOG_LEVEL_WARN);
                  return;
                }  
                
                //exponent
                if(lwp_get_attr_value(cb->buf, &d->wkc.channel[j], LWP_ATTR_CHANNEL_EXPONENT, ex, CHANNEL_ATTR_STR_LEN)){
                  snprintf(log, LOG_BUF_LEN, "failed to parse .well-known/core: /channel%i/exponent", j);
                  lielas_log((unsigned char*)log, LOG_LEVEL_WARN);
                  return;
                }  
                // parse exponent to double
                c[j]->exponent = strtod(ex, NULL);
                if(c[j]->exponent == 0.0){
                  snprintf(log, LOG_BUF_LEN, "failed to parse .well-known/core: /channel%i/exponent", j);
                  lielas_log((unsigned char*)log, LOG_LEVEL_WARN);
                  return;
                }
                
								LaddChannel(m[i], c[j]);
                snprintf(log, LOG_BUF_LEN, "found channel%i with unit %s, type %s and class %s", j, c[j]->unit, c[j]->type, c[j]->class);
                lielas_log((unsigned char*) log, LOG_LEVEL_DEBUG);
                
            }
          }
          
          // basic device settings finished, save device
          lielas_log((unsigned char*)"saving new Device:", LOG_LEVEL_DEBUG);
          
					LDCadd(d);
					LDCsaveNewDevice(d);
          
          LprintDeviceStructure(d, log, LOG_BUF_LEN, 0);
          lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);
          
          
					LDCsaveUpdatedDevice(d);
          LDCsaveWKC(wkc, d);
          
          // set datetime
          lielas_log((unsigned char*)"setting device date and time", LOG_LEVEL_DEBUG);
          DeviceSetDatetime(d);
          
          // set logger interval
          lielas_log((unsigned char*)"setting device logger interval to 600", LOG_LEVEL_DEBUG);
          snprintf(cmd, DATABUFFER_SIZE, "coap://[%s]:5683/logger", d->address);
          snprintf((char*)payload, DATABUFFER_SIZE, "interval=600");
          coap_send_cmd(cmd, cb, MYCOAP_METHOD_PUT, payload);
          
          
          // set logger state
          lielas_log((unsigned char*)"setting device logger on", LOG_LEVEL_DEBUG);
          snprintf(cmd, DATABUFFER_SIZE, "coap://[%s]:5683/logger", d->address);
          snprintf((char*)payload, DATABUFFER_SIZE, "state=1");
          coap_send_cmd(cmd, cb, MYCOAP_METHOD_PUT, payload);
          
          //put device to sleep
          setCycleMode(d, LWP_CYCLE_MODE_ON);
          
        }
      }
		}
    nextToken += 1;
	}

}

int ipv6ToMac(const char* ip, char *mac){
  struct in6_addr ipv6;
  
  if(inet_pton(AF_INET6, ip, &ipv6) != 1){
    return -1;
  }

  snprintf(mac, MAC_STR_LEN, "00:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X", ipv6.s6_addr[9], ipv6.s6_addr[10],
                                                                      ipv6.s6_addr[11], ipv6.s6_addr[12], 
                                                                      ipv6.s6_addr[13], ipv6.s6_addr[14], 
                                                                      ipv6.s6_addr[15]);
  
  return 0;
} 







