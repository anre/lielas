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


static struct Ldc_struct *deviceContainer = NULL;
static struct Ldc_struct *dcPtr;

void loadModuls(Ldevice *d, char *moduls);
void loadChannels(Lmodul *m, char *channels);



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
	char st[SQL_STATEMENT_BUF_SIZE];

	//query devices table
	snprintf(st, SQL_STATEMENT_BUF_SIZE, "SELECT id, address, mint, pint, aint, moduls, registered FROM %s.%s", LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICES);
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

		d->id = atoi(PQgetvalue(res, i, 0));
		//d->registered = 1;
		strcpy(d->address, PQgetvalue(res,i, 1));
		strcpy(d->mint, PQgetvalue(res, i, 2));
		strcpy(d->pint, PQgetvalue(res, i, 3));
		strcpy(d->aint, PQgetvalue(res, i, 4));
		strcpy(moduls,  PQgetvalue(res, i, 5));
		loadModuls(d, moduls);
		LDCadd(d);
		sprintf(msg, "Device %s loaded", d->address);
		lielas_log((unsigned char*)msg, LOG_LEVEL_DEBUG);

	}
	PQclear(res);
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
		            "SELECT id, address, channels FROM %s.%s WHERE id=%s",
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
                "SELECT id, address, type, unit FROM %s.%s WHERE id=%s",
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

	snprintf(st, SQL_BUFFER_SIZE, "SELECT id, address, mint, pint, aint, moduls, registered FROM %s.%s WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICES, id);
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
	strcpy(d->mint, PQgetvalue(res, 0, 2));
	strcpy(d->pint, PQgetvalue(res, 0, 3));
	strcpy(d->aint, PQgetvalue(res, 0, 4));
	strcpy(moduls,  PQgetvalue(res, 0, 5));
	loadModuls(d, moduls);
	PQclear(res);

	return d;
}

/********************************************************************************************************************************
 * 		LDCsaveNewDevice: write new device to database
 ********************************************************************************************************************************/

int LDCsaveNewDevice(Ldevice *d){
	PGresult *res;
	char st[SQL_BUFFER_SIZE];
	int id;

	id = getNewId(ID_TYPE_DEVICE);
	if(id == 0){
		lielas_log((unsigned char*)"Can't get new device id", LOG_LEVEL_WARN);
		return -1;
	}

	snprintf(st, 200, "INSERT INTO %s.%s (id, address, registered, mint, pint, aint) VALUES (%d, '%s', '%s', '%s', '%s', '%s')",
	         LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICES, id, d->address, "false", d->mint, d->pint, d->aint);

	res = SQLexec(st);

	PQclear(res);

	printf("Device %s gespeichert \n", d->address);
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

	for(i = 0; i < MAX_MODULS && d->modul[i] != NULL; i++){
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
			snprintf(st, SQL_BUFFER_SIZE, "SELECT %s.%s FROM devices WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_MODULS , d->id);
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
		for(j = 0; j < MAX_CHANNELS && d->modul[i]->channel[j] != NULL; j++){
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
				snprintf(st, SQL_BUFFER_SIZE, "INSERT INTO channels (id, address, type, unit) VALUES (%d, '%s', '%s', '%s')",
						 c->id, c->address, c->type, c->unit);
				res = SQLexec(st);
				if(!res){
					lielas_log((unsigned char*)"Error executing SQL statement", LOG_LEVEL_WARN);
					PQclear(res);
					return -1;
				}
				PQclear(res);

				//update modul-channel entry
				snprintf(st, SQL_BUFFER_SIZE, "SELECT %s.%s FROM moduls WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_MODULS , d->modul[i]->id);
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
	//static int j = 0;

	int i;
	int state = 0;
	int eof = 0;
	int adrLen = 0;
	char cmd[CLIENT_BUFFER_LEN];
	char buf[CLIENT_BUFFER_LEN];
	char adr[IPV6_ADR_BUF_LEN];
	char *ptr = 0;
	FILE *file;
	Ldevice *d;
	Lmodul *m[MAX_MODULS];
	Lchannel *c[MAX_CHANNELS];
	time_t rawtime;
	struct tm *now;
	static struct tm *nextScan;
	double diff;

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

	//create command
	sprintf(cmd, "wget -T 2 --tries=1 -O gatewayentries.html http://[%s]:%s ", set_getGatewaynodeAddr(), set_getGatewaynodePort());

	system("rm gatewayentries.html");
	system(cmd);


	file = fopen("gatewayentries.html", "r");
	if(file == NULL){
		lielas_log((unsigned char*)"Can't connect to Gateway", LOG_LEVEL_WARN);
		return;
	}

	while(fgets(buf, CLIENT_BUFFER_LEN, file) && !eof){
		if(state == 0){ // "Routes" not yet found
			ptr = strstr(buf, "Routes");
			if(ptr){
				state = 1;
			}
		}
		if(state == 1){	// "Routes" found, read addresses
			ptr = strstr(buf, "<pre>");	// go to line end
			if(ptr){
				ptr += 5;
				state = 2;
			}
		}
		if(state == 2){	// parses address
			for( i = 0; i < IPV6_ADR_BUF_LEN && !eof ; i++){
				if(ptr[i] == '<'){
					eof=1;
					break;
				}
				if(ptr[i] == '/'){
					if(i < IPV6_ADR_BUF_LEN){
						adr[adrLen] = 0;
						adrLen = 0;
						if(LDCgetDeviceByAddress(adr, &d) == 0){
							if(d == NULL){	// new device
								printf("New Device found: %s\n", adr);

								d = LcreateDevice();
								strcpy(d->address, adr);
								sprintf(d->mint, "%d", LIELAS_STD_MINT);
								sprintf(d->pint, "%d", LIELAS_STD_PINT);
								sprintf(d->aint, "%d", LIELAS_STD_AINT);

								LDCadd(d);
								LDCsaveNewDevice(d);

								m[0] = LcreateModul();
								sprintf(m[0]->address, "1");
								sprintf(m[0]->mint, "%d", LIELAS_STD_MINT);
								sprintf(m[0]->pint, "%d", LIELAS_STD_PINT);
								sprintf(m[0]->aint, "%d", LIELAS_STD_AINT);
								LaddModul(d, m[0]);

								c[0] = LcreateChannel();
								sprintf(c[0]->address, "1");
								sprintf(c[0]->type, "SHT_T");
								sprintf(c[0]->unit, "°C");
								LaddChannel(m[0], c[0]);

								c[1] = LcreateChannel();
								sprintf(c[1]->address, "2");
								sprintf(c[1]->type, "SHT_H");
								sprintf(c[1]->unit, "%%");
								LaddChannel(m[0], c[1]);

								c[2] = LcreateChannel();
								sprintf(c[2]->address, "3");
								sprintf(c[2]->type, "LPS_P");
								sprintf(c[2]->unit, "mbar");
								LaddChannel(m[0], c[2]);

								LDCsaveUpdatedDevice(d);
							}
						}
						break;
					}
				}else{
					adr[adrLen++] = ptr[i];
				}
			}
			ptr = buf;
		}
	}
	pclose(file);

	//j = 1;
}





