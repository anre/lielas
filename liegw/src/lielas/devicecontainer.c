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
#include "../sql/mysqlhandler.h"
#include "../settings.h"
#include "ldb.h"
#include "../jsmn/jsmn.h"
#include "../coap/libcoap/coap.h"
#include "../coap/mycoap.h"
#include "lwp.h"
#include "../devicehandler.h"


static struct Ldc_struct *deviceContainer = NULL;
static struct Ldc_struct *dcPtr;
static int nrOfDevices;

void loadModuls(Ldevice *d, char *moduls);
void loadChannels(Lmodul *m, char *channels);

int ipv6ToMac(const char* ip, char *mac);
int convertHtmlToJson(char *html);

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

  nrOfDevices  = 0;
  
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
        nrOfDevices  = 0;
				return;
			}
			dc = dc->ndc;
		}
		free(deviceContainer);
    nrOfDevices  = 0;
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
    nrOfDevices  += 1;
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
    nrOfDevices  += 1;
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
    nrOfDevices  += 1;
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
  nrOfDevices  -= 1;
	return 0;
}

int LDCgetNumberOfDevices(){
  return nrOfDevices;
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
  char log[LOG_BUF_LEN];
  char buf[SQL_STATEMENT_BUF_SIZE];

	//query devices table
  
	snprintf(st, SQL_STATEMENT_BUF_SIZE, "SELECT id, address, mac, mint, moduls, registered, datapakets, router, datalogger, sw_ver, v_source, v_source_state, route_id FROM %s.%s", LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICES);
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
		d->id = strtol(PQgetvalue(res, i, 0), NULL, 10);
		//d->registered = 1;
		strcpy(d->address, PQgetvalue(res,i, 1));
		strcpy(d->mac, PQgetvalue(res, i, 2));
		strcpy(d->mint, PQgetvalue(res, i, 3));
		strcpy(moduls,  PQgetvalue(res, i, 4));
    d->datapakets = strtol(PQgetvalue(res, i, 6), NULL, 10);
    //router
    strcpy(buf, PQgetvalue(res, i, 7));
    if(!strcmp(buf, "t")){
      d->router = 1;
    }else{
      d->router = 0;
    }
    //datalogger
    strcpy(buf, PQgetvalue(res, i, 8));
    if(!strcmp(buf, "t")){
      d->datalogger = 1;
    }else{
      d->datalogger = 0;
    }
    //sw_ver
    strcpy(d->sw_ver, PQgetvalue(res, i, 9));
    //v_source
    strcpy(d->supply, PQgetvalue(res, i, 10));
    //v_source_state
    strcpy(d->supplyState, PQgetvalue(res, i, 11));
    //route_id
    d->route_id = strtol(PQgetvalue(res, i, 12), NULL, 10);
    
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
                "SELECT id, address, type, unit, exponent, class FROM %s.%s WHERE id=%s",
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
			strcpy(channel->class, PQgetvalue(res, 0, 5));
      
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
  char buf[SQL_STATEMENT_BUF_SIZE];

	d = LcreateDevice();

	if(d == NULL){
		return d;
	}

	snprintf(st, SQL_BUFFER_SIZE, "SELECT id, address, mac, mint, moduls, registered, datapakets, router, datalogger, sw_ver, v_source, v_source_state, route_id FROM %s.%s WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICES, id);
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
	strcpy(moduls,  PQgetvalue(res, 0, 4));
  d->datapakets = strtol(PQgetvalue(res, 0, 6), NULL, 10);
  //router
  strcpy(buf, PQgetvalue(res, 0, 7));
  if(!strcmp(buf, "t")){
    d->router = 1;
  }else{
    d->router = 0;
  }
  //datalogger
  strcpy(buf, PQgetvalue(res, 0, 8));
  if(!strcmp(buf, "t")){
    d->datalogger = 1;
  }else{
    d->datalogger = 0;
  }
  //sw_ver
  strcpy(d->sw_ver, PQgetvalue(res, 0, 9));
  //v_source
  strcpy(d->supply, PQgetvalue(res, 0, 10));
    //v_source_state
  strcpy(d->supplyState, PQgetvalue(res, 0, 11));
  //route_id
  d->route_id = strtol(PQgetvalue(res, 0, 12), NULL, 10);
  
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
 * 		LDCsaveNrOfDatapaktes: 
 ********************************************************************************************************************************/
int LDCsaveNrOfDatapakte(Ldevice *d){
	PGresult *res;
	char st[SQL_STATEMENT_BUF_SIZE];
  
  snprintf(st, SQL_STATEMENT_BUF_SIZE, "UPDATE %s.%s SET datapakets='%d' WHERE id=%u", LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICES, d->datapakets, d->id);
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

	snprintf(st, SQL_BUFFER_SIZE, "INSERT INTO %s.%s (id, address, mac, registered, name,  mint, datapakets, router, datalogger, v_source, v_source_state, sw_ver, route_id) VALUES (%d, '%s', '%s', '%s', '%s', '%s', '%d', '%d', '%d', '%s', '%s', '%s', '%d')",
	         LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICES, id, d->address, d->mac, "true", d->name, d->mint, d->datapakets, d->router, d->datalogger, d->supply, d->supplyState, d->sw_ver, d->route_id);

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
		snprintf(st, SQL_BUFFER_SIZE, "UPDATE %s.%s SET address='%s' WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICES , d->address, d->id);
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
		snprintf(st, SQL_BUFFER_SIZE, "UPDATE %s.%s SET mint='%s' WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICES , d->mint, d->id);
		res = SQLexec(st);
		if(!res){
			lielas_log((unsigned char*)"Error executing SQL statement", LOG_LEVEL_WARN);
			PQclear(res);
			return -1;
		}
		PQclear(res);
	}
  if(strcmp(d->supply, oldDevice->supply)){
		snprintf(st, SQL_BUFFER_SIZE, "UPDATE %s.%s SET v_source='%s' WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICES , d->supply, d->id);
		res = SQLexec(st);
		if(!res){
			lielas_log((unsigned char*)"Error executing SQL statement", LOG_LEVEL_WARN);
			PQclear(res);
			return -1;
		}
		PQclear(res);
  }
  if(strcmp(d->supplyState, oldDevice->supplyState)){
		snprintf(st, SQL_BUFFER_SIZE, "UPDATE %s.%s SET v_source_state='%s' WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICES , d->supplyState, d->id);
		res = SQLexec(st);
		if(!res){
			lielas_log((unsigned char*)"Error executing SQL statement", LOG_LEVEL_WARN);
			PQclear(res);
			return -1;
		}
		PQclear(res);
  }
  if(strcmp(d->sw_ver, oldDevice->sw_ver)){
		snprintf(st, SQL_BUFFER_SIZE, "UPDATE %s.%s SET sw_ver='%s' WHERE id=%d", LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICES , d->sw_ver, d->id);
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
			snprintf(st, SQL_BUFFER_SIZE, "INSERT INTO %s.%s (id, address, mint) VALUES (%d, '%s', '%s')",
			         LDB_TBL_SCHEMA, LDB_TBL_NAME_MODULS , d->modul[i]->id, d->modul[i]->address, d->modul[i]->mint);

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
  int devices = 0;
  int routes  = 0;
  Ldevice *d;
  Lmodul *m[MAX_MODULS];
  Lchannel *c[MAX_CHANNELS];
  IGROUTE igroutes[MAX_DEVICES];
  char log[LOG_BUF_LEN];
  int i;
  
	//get number of actual devices
  devices = LDCgetNumberOfDevices();
  
  //get number of routes;
  routes = msql_get_number_of_routes();
  
  if(routes <= devices){
    return;
  }
  
  //load all routes
  routes = msql_get_routes(igroutes, MAX_DEVICES);
  
  //new devices found
  while(routes > devices){
  
    //search the new route
    for(i = 0; i < routes; i++){
      d = LDCgetFirstDevice();
      while(d != NULL){
        //route allready registered, skip it
        if(d->route_id == igroutes[i].id){
          break;
        }
        d = LDCgetNextDevice();
      }
      
      if(d == NULL || d->route_id != igroutes[i].id){
        break;
      }
    }
    
    
    if(d == NULL || d->route_id != igroutes[i].id){
    
      //new route found
      printf("new route found: %s\n", igroutes[i].route);
      
      d = LcreateDevice();
      d->id = igroutes[i].id;
      d->route_id = igroutes[i].id;
      strcpy(d->address, igroutes[i].route);	
      sprintf(d->mint, "%d", LIELAS_STD_MINT);
      snprintf(log, LOG_BUF_LEN, "new device found: %s", d->address);
      lielas_log((unsigned char*) log, LOG_LEVEL_DEBUG);
        
      //d->name
      strcpy(d->name, "mini2TH");
          
      //d->sw_ver
      strcpy(d->sw_ver, "0.1");
      
      //d->router
      d->router = 0;
      strcpy(d->supply, "bat");
      strcpy(d->supplyState, LIELAS_SUPPLY_STATE_OK);
      
      d->datalogger = 1;
      
      m[1] = LcreateModul();
      sprintf(m[1]->address, "1");
      sprintf(m[1]->mint, "%d", LIELAS_STD_MINT);
      
      c[1] = LcreateChannel();
      sprintf(c[1]->address, "1");
      sprintf(c[1]->unit, "°C");
      sprintf(c[1]->class, "SENSOR");
      sprintf(c[1]->type, "SHT_T");
      c[1]->exponent = -1.0;
      LaddChannel(m[1], c[1]);
            
      c[2] = LcreateChannel();
      sprintf(c[2]->address, "2");
      sprintf(c[2]->unit, "%%");
      sprintf(c[2]->class, "SENSOR");
      sprintf(c[2]->type, "SHT_RH");
      c[2]->exponent = -1.0;
      LaddChannel(m[1], c[2]);
            
      c[3] = LcreateChannel();
      sprintf(c[3]->address, "3");
      sprintf(c[3]->unit, "g/m³");
      sprintf(c[3]->class, "SENSOR");
      sprintf(c[3]->type, "SHT_AH");
      c[3]->exponent = -1.0;
      LaddChannel(m[1], c[3]);
            
      c[4] = LcreateChannel();
      sprintf(c[4]->address, "4");
      sprintf(c[4]->unit, "°C");
      sprintf(c[4]->class, "SENSOR");
      sprintf(c[4]->type, "SHT_DP");
      c[4]->exponent = -1.0;
      LaddChannel(m[1], c[4]);
      
      LaddModul(d, m[1]);
      
      if(ipv6ToMac(igroutes[i].route, d->mac)){
        return;
      }
      
      LDCadd(d);
      
      lielas_log((unsigned char*)"saving new Device:", LOG_LEVEL_DEBUG);
      LDCsaveNewDevice(d);
      LDCsaveUpdatedDevice(d);
      
      LprintDeviceStructure(d, log, LOG_BUF_LEN, 0);
      
    }
    devices += 1;
  }
  
}



/********************************************************************************************************************************
 * 		int ipv6ToMac(const char* ip, char *mac)
 ********************************************************************************************************************************/

int ipv6ToMac(const char* ip, char *mac){
  struct in6_addr ipv6;
  
  if(inet_pton(AF_INET6, ip, &ipv6) != 1){
    return -1;
  }

  snprintf(mac, MAC_STR_LEN, "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X"
            , ipv6.s6_addr[8] , ipv6.s6_addr[9] , ipv6.s6_addr[10], ipv6.s6_addr[11]
            , ipv6.s6_addr[12], ipv6.s6_addr[13], ipv6.s6_addr[14], ipv6.s6_addr[15]);
  
  return 0;
} 


int convertHtmlToJson(char *html){
  size_t len;
  size_t jsonLen;
  char *ptr;
  int i;
  char json[RPL_TABLE_LEN];
  int eof = 0;
  
  ptr = strstr(html, "Routes");
  
  if( ptr == NULL){
    lielas_log((unsigned char*)"Failed to convert HTML page to JSON: Token 'Routes' not found!", LOG_LEVEL_WARN);
    return -1;
  }
  
  len = strlen(ptr);
    
  //search first address
  for(i = 0; i < len; i++){
    if(ptr[i] == '>')
      break;
  }
  
  if( i == len ){
    lielas_log((unsigned char*)"Failed to convert HTML page to JSON: no address found!", LOG_LEVEL_WARN);
    return -1;
  }
  
  //address found, init json
  snprintf(json, RPL_TABLE_LEN, "{\n\"Routes\":[\n");
  jsonLen = strlen(json);
  
  ptr = &ptr[i+1];
  
  while(jsonLen < RPL_TABLE_LEN && !eof){
    //address found, copy it to json
    json[jsonLen++] = ' ';
    json[jsonLen++] = ' ';
    json[jsonLen++] = '"';
    for( i = 0; jsonLen <  RPL_TABLE_LEN; i++){
      if(ptr[i] == '/'){
        break;
      }
      json[jsonLen++] = ptr[i];
    }
    json[jsonLen++] = '"';
    json[jsonLen++] = ',';
    json[jsonLen++] = '\n';
    
    //search next line
    for( i = 0; i < len; i++){
      if(ptr[i] == 0){
        eof = 1;
        break;
      }
      if(ptr[i] == '\n'){
        i += 1;
        break;
      }
    }
    //check for new address
    if(!eof){
      if(ptr[i] == '<'){
        eof = 1;
      }      
    }
    ptr += i;
  }
  
  if(jsonLen > 2) //clear last koma
    jsonLen -= 2;
    
  snprintf(&json[jsonLen], RPL_TABLE_LEN, "\n  ]\n}");
  jsonLen = strlen(json);
  
  //copy json back to html buffer
  for(i = 0; i < jsonLen  && i < RPL_TABLE_LEN; i++){
    html[i] = json[i];
  }
  
  if(i < RPL_TABLE_LEN){
    html[i] = 0;
  }else{
    lielas_log((unsigned char*)"Failed to convert HTML page to JSON: buffer too small!", LOG_LEVEL_WARN);
    return -1;
  }
  
  return 0;
}






