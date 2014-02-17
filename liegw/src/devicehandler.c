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


#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "devicehandler.h"
#include "lielas/devicecontainer.h"
#include "lielas/deviceevent.h"
#include "lielas/lielas.h"
#include "coap/libcoap/coap.h"
#include "sql/sql.h"
#include "jsmn/jsmn.h"
#include "coap/mycoap.h"
#include "settings.h"
#include "log.h"
#include "lielas/ldb.h"


int getDeviceData(Ldevice *d, datapaketcontainer *dpc, int offset);
int SaveDataPaketContainerToDatabase(datapaketcontainer* dpc);
datapaketcontainer *CreateDatapaketcontainer();
void DeleteDatapaketcontainer(datapaketcontainer *dpc);
int DeviceSetDatetime(Ldevice *d);
datapaket *CreateDatapaket();
int setLoggerMint(Ldevice *d, Lmodul *m);
int convertValueWithExponent(char *indata, char *outval, uint8_t outValLen, double ex);
uint16_t get_crc(unsigned char* data, int len);
int getNumberOfDatasets(Ldevice *d);
int handleRtdbgsys(Ldevice* d);

/********************************************************************************************************************************
 * 		void HandleDevices(): handle device events
 ********************************************************************************************************************************/

void HandleDevices(){
  Ldevice *d;
  datapaketcontainer *dpc;
  time_t rawtime;
  struct tm *now;
  struct tm *endTime;
  struct tm *endOfTimeslot = NULL;
  int numberOfDevices;
  int timeslotDuration;
  char log[LOG_BUF_LEN];
  int datapakets;
  static int lastHour;
  
  //check if it is time to get data
  time(&rawtime);
  now = gmtime(&rawtime);
  
  if(now->tm_min != 2 || now->tm_hour == lastHour){
    return;
  }
  lastHour = now->tm_hour;
  
  //get number of devices
  numberOfDevices = LDCgetNumberOfDevices();
  if(numberOfDevices < 1){
    return;
  }
  
  //calculate endTime and timeslots
  endTime = malloc(sizeof(struct tm));
  if(endTime == NULL){
    lielas_log((unsigned char*)"error reserving space for struct tm endTime", LOG_LEVEL_ERROR);
    return;
  }
  *endTime = *now;
  endTime->tm_min = 4;
  endTime->tm_sec = 0;
  
  timeslotDuration = (int)(difftime(mktime(endTime), mktime(now)) / numberOfDevices);
  if(now->tm_hour == 0){
    timeslotDuration -= 2;
  }
  
  snprintf(log, LOG_BUF_LEN, "Starting to get data. %i devices, timeslots are %i s long", numberOfDevices, timeslotDuration);
  lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);
  
  //get first device
  d = LDCgetFirstDevice();
  if(d == NULL) 
    return;
  
  while(d != NULL){
    //TODO get rtdbgsys
    //0 o'clock? test date/time
    if(now->tm_hour == 0){
      DeviceSetDatetime(d);
    }
    handleRtdbgsys(d);
  
    //calculate end of timeslot
    if(endOfTimeslot == NULL){
      //first device
      endOfTimeslot = malloc(sizeof(struct tm));
      if(endOfTimeslot == NULL){
        lielas_log((unsigned char*)"error reserving space for struct tm endOfTimeslot", LOG_LEVEL_ERROR);
        free(endTime);
        return;
      }
      *endOfTimeslot = *now;
      endOfTimeslot->tm_sec += timeslotDuration;
      mktime(endOfTimeslot);
    }else{
      endOfTimeslot->tm_sec += timeslotDuration;
      mktime(endOfTimeslot);
    }
    
    //get number of datasets
    datapakets = getNumberOfDatasets(d);
    snprintf(log, LOG_BUF_LEN, "Get data from %s with %i/%i datapakets\n", d->mac, d->datapakets ,datapakets);
    lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);
    
    if(d->datapakets < datapakets){ //only get data if there is new data
      //get datapakets until all data or end of timeslot
      while((difftime(mktime(endOfTimeslot), mktime(now)) > 0) && (d->datapakets < datapakets)){
        dpc = CreateDatapaketcontainer();
        if(dpc == NULL){
          lielas_log((unsigned char*)"error reserving space for datapaket container", LOG_LEVEL_ERROR);
          free(endTime);
          free(endOfTimeslot);
          return;
        }
        if(!getDeviceData(d, dpc, d->datapakets)){
          d->datapakets += dpc->datapakets / dpc->m->channels;
          if(SaveDataPaketContainerToDatabase(dpc)){
            lielas_log((unsigned char*)"failed to save datapakets", LOG_LEVEL_ERROR);
            d->datapakets -= dpc->datapakets / dpc->m->channels;
          }
        }
        DeleteDatapaketcontainer(dpc);
        time(&rawtime);
        now = gmtime(&rawtime);
      }
    }
        
    //check date/time
    time(&rawtime);
    now = gmtime(&rawtime);
    if(difftime(mktime(endTime), mktime(now)) <= 0.){  //end of time
      free(endTime);
      free(endOfTimeslot);
      return;
    }
    
    //get next device
    d = LDCgetNextDevice();
  }
  free(endTime);
  free(endOfTimeslot);
}

/********************************************************************************************************************************
 * 		int testDatetime()
 ********************************************************************************************************************************/
int testDatetime(struct tm *dt){
	if(dt->tm_hour < 0 || dt->tm_hour > 23)
		return 0;
	if(dt->tm_min < 0 || dt->tm_min > 59)
		return 0;
	if(dt->tm_sec < 0 || dt->tm_sec > 59)
		return 0;
	if(dt->tm_year < 100 || dt->tm_year > 200)
		return 0;
	if(dt->tm_mon < 0 || dt->tm_mon > 11)
		return 0;
	if(dt->tm_mday < 1 || dt->tm_mday > 31)
		return 0;

	return 1;
}

int getNumberOfDatasets(Ldevice *d){
  coap_buf *cb;     
  char cmd[CMDBUFFER_SIZE];  
  unsigned char buf[DATABUFFER_SIZE] = { 0 }; 
  char log[LOG_BUF_LEN];
  int datasets;
  
  lielas_log((unsigned char*) "get number of datasets", LOG_LEVEL_DEBUG);
  
  cb = coap_create_buf();
  if( cb == NULL){
    snprintf(log, LOG_BUF_LEN, "failed to create coap buffer");
    lielas_log((unsigned char*)log, LOG_LEVEL_ERROR);
    return -1;
  }
  cb->buf = (char*)buf;
  cb->bufSize = DATABUFFER_SIZE;
  
  snprintf(cmd, DATABUFFER_SIZE, "coap://[%s]:5683/database", d->address);        
  coap_send_cmd(cmd, cb, MYCOAP_METHOD_GET, NULL);    
  
  if(cb->status != COAP_STATUS_CONTENT){
    lielas_log((unsigned char*) "Status error getting number of datasets", LOG_LEVEL_DEBUG);
    return -1;
  }
  
  datasets = (int)strtol(cb->buf, NULL, 10);
  return datasets;
}

/********************************************************************************************************************************
 * 		int getDeviceData(Ldevice *d): get one set of datapakets
 ********************************************************************************************************************************/
int getDeviceData(Ldevice *d, datapaketcontainer *dpc, int offset){
  coap_buf *cb;        
  unsigned char buf[DATABUFFER_SIZE] = { 0 };
  char cmd[CMDBUFFER_SIZE];
  char log[LOG_BUF_LEN];
  struct tm dt;
  int pos = 0;
  int cnr;        
  datapaket *dp;
  int16_value_t val;
  crc_t crc;

  snprintf(log, LOG_BUF_LEN, "\nget device data from %s", d->mac);
  lielas_log((unsigned char*) log, LOG_LEVEL_DEBUG);
  
  dpc->datapakets = 0;
  dpc->dec = 0;
  dpc->d = d;
  dpc->m = d->modul[1];
  
  cb = coap_create_buf();
  if( cb == NULL){
    snprintf(log, LOG_BUF_LEN, "failed to create coap buffer");
    lielas_log((unsigned char*)log, LOG_LEVEL_ERROR);
    return -1;
  }
  cb->buf = (char*)buf;
  cb->bufSize = DATABUFFER_SIZE;
  
  snprintf(cmd, DATABUFFER_SIZE, "coap://[%s]:5683/database?offset=%d;", d->address, offset);        
  coap_send_cmd(cmd, cb, MYCOAP_METHOD_GET, NULL);
  
  if(cb->status != COAP_STATUS_CONTENT){
    lielas_log((unsigned char*) "Status error getting data", LOG_LEVEL_DEBUG);
    return -1;
  }
  
  snprintf(log, LOG_BUF_LEN, "paket:\n");
  for(pos = 0; pos < cb->len; pos++){
    snprintf(&log[strlen(log)], LOG_BUF_LEN, "%x", (unsigned char)cb->buf[pos]);
    if(((pos + 1) % 10) == 0){
      snprintf(&log[strlen(log)], LOG_BUF_LEN,"\n");
    }
  }
  lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);
  pos = 0;
  
  while(pos < cb->len){
    
    uint16_t calcCrc = get_crc((unsigned char*)&cb->buf[pos], 8);
    crc.uh = cb->buf[pos+9];
    crc.ul = cb->buf[pos + 8];
    
    if(crc.u16 != calcCrc){
        snprintf(log, LOG_BUF_LEN, "crc error: received %x but calculated %x ", (uint16_t)crc.u16, (uint16_t)calcCrc);
        lielas_log((unsigned char*)log, LOG_LEVEL_ERROR);
        pos += 10;
    }else{
      lwp_compdt_to_struct_tm((unsigned char*)&cb->buf[pos], &dt);
      mktime(&dt);
      pos += 4;
      
      snprintf(log, LOG_BUF_LEN, "found datetime: %x:%x:%x:%x ... ", (unsigned char)cb->buf[pos - 4], (unsigned char)cb->buf[pos - 3], (unsigned char)cb->buf[pos - 2], (unsigned char)cb->buf[pos - 1]);
      strftime(&log[strlen(log)], LOG_BUF_LEN, "%d.%m.%Y %H:%M:%S",&dt);
      lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);
      
      if(testDatetime(&dt)){
        for(cnr = 1; cnr <= d->modul[1]->channels; cnr++){
          dp = CreateDatapaket();
          if(dp == NULL){
            break;
          }
          //init datapaket
          dp->d = d;
          dp->m = d->modul[1];
          dp->c = d->modul[1]->channel[cnr];
          //build value
          val.uh = cb->buf[pos + 1];
          val.ul = cb->buf[pos];
          //convert value to string
          double dval = (double)val.val / 100.;
          snprintf(dp->value, VALUEBUFFER_SIZE, "%.2f", (double)dval);
          //change . to , in string
          char komma[]  = ".";
          int kommaPos = strcspn(dp->value, komma);
          if(komma > 0){
            dp->value[kommaPos] = ',';
          }
          pos += 2;
          memcpy(dp->dt, &dt, sizeof(struct tm));
          dpc->dp[dpc->datapakets] = dp;
          dpc->datapakets += 1;
          snprintf(log, LOG_BUF_LEN, "found value: %x:%x ... %s", (unsigned char)cb->buf[pos-2], (unsigned char)cb->buf[pos-1], dp->value);
          lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);
        }
      }
      pos += 2;
    }
  }
	return 0;
}

/********************************************************************************************************************************
 * 		int DeviceSetTime(Ldevice *d):
 * 			set datetime entry on device
 ********************************************************************************************************************************/
int DeviceSetDatetime(Ldevice *d){
	time_t rawtime;
	struct tm *now;
	struct tm dt;
	char cmd[CMDBUFFER_SIZE];
	char datetime[CMDBUFFER_SIZE];
  char recvDt[CMDBUFFER_SIZE];
	char buf[CMDBUFFER_SIZE];
  char log[CMDBUFFER_SIZE];
	coap_buf *cb = coap_create_buf();

	if(cb == NULL){
		return -1;
	}
	cb->buf = buf;
  cb->bufSize = CMDBUFFER_SIZE;

	if(d == NULL){
		return -1;
	}

	time(&rawtime);
	now = gmtime(&rawtime);
  
  
	snprintf(cmd, DATABUFFER_SIZE, "coap://[%s]:5683/datetime", d->address);
	strftime(datetime, DATABUFFER_SIZE, "datetime=%Y.%m.%d-%H:%M:%S", now);
	coap_send_cmd(cmd, cb, MYCOAP_METHOD_PUT, (unsigned char*)datetime);
	sleep(1);

	//get time and check if it is set
	buf[0] = 0;
	snprintf(cmd, DATABUFFER_SIZE, "coap://[%s]:5683/datetime", d->address);
	coap_send_cmd(cmd, cb, MYCOAP_METHOD_GET, NULL);
  
  if(cb->status != COAP_STATUS_CONTENT){
    lielas_log((unsigned char*) "Status error setting datetime\n", LOG_LEVEL_DEBUG);
		return -1;
	}
  
  dt.tm_isdst = 0;
	strptime(cb->buf, "%Y.%m.%d-%H:%M:%S", &dt);
  snprintf(log, CMDBUFFER_SIZE, "device time: %s", recvDt);
  lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);
  
	time(&rawtime);
	now = gmtime(&rawtime);
  
	if(difftime(mktime(now), mktime(&dt)) < MAX_TIME_DIFF){
		return 0;
	}
  
  lielas_log((unsigned char*)"setting time: time difference too big", LOG_LEVEL_WARN);
  snprintf(log, CMDBUFFER_SIZE, "server time: ");
	strftime(&log[strlen(log)], CMDBUFFER_SIZE - strlen(log), "%d.%m.%Y-%H:%M:%S", now);
  snprintf(&log[strlen(log)], CMDBUFFER_SIZE - strlen(log), " received time: ");
	strftime(&log[strlen(log)], CMDBUFFER_SIZE - strlen(log), "%d.%m.%Y-%H:%M:%S",&dt);
  lielas_log((unsigned char*)log, LOG_LEVEL_WARN);
	return -1;
}


/********************************************************************************************************************************
 * 		int SaveDataPaketContainerToDatabase(datapaketcontainer* dpc):
 * 			save dpc to database
 ********************************************************************************************************************************/
int SaveDataPaketContainerToDatabase(datapaketcontainer* dpc){
	//PGresult *res;
	int i;
	char st[CMDBUFFER_SIZE];
	PGresult *res;
	char dtStr[CMDBUFFER_SIZE];
	char adrStr[CMDBUFFER_SIZE];
  char log[LOG_BUF_LEN];
	int success = 0;

	if(SQLTableExists(LDB_TBL_NAME_DATA)){
		lielas_createDataTbl();
		if(SQLTableExists(LDB_TBL_NAME_DATA)){
			return -1;
		}
	}

  snprintf(log, LOG_BUF_LEN, "saving values to database");
	lielas_log((unsigned char*) log, LOG_LEVEL_DEBUG);
  
	// create column string
	//snprintf(column, BUFFER_SIZE, "%s.%s.%s", dpc->d->address, dpc->m->address, dpc->c->address);

	for( i = 0; i < dpc->datapakets; i++){

		//create column if not existing
		snprintf(adrStr, CMDBUFFER_SIZE, "%s.%s.%s", dpc->dp[i]->d->mac, dpc->dp[i]->m->address, dpc->dp[i]->c->address);
		if(SQLRowExists(LDB_TBL_NAME_DATA, adrStr)){
			snprintf(st, CMDBUFFER_SIZE, "ALTER TABLE %s.%s ADD COLUMN \"%s\" text", LDB_TBL_SCHEMA, LDB_TBL_NAME_DATA , adrStr);
			res = SQLexec(st);
			PQclear(res);
		}

		// create datetime string
		strftime(dtStr, CMDBUFFER_SIZE, "%Y-%m-%d %H:%M:%S", dpc->dp[i]->dt);
    
		//check, if datetime-row exists
		if(SQLCellExists(LDB_TBL_NAME_DATA, "datetime", dtStr)){
			//creat new row
			snprintf(st, CMDBUFFER_SIZE, "INSERT INTO %s.%s ( datetime, \"%s\" ) VALUES ( '%s', '%s')", 
                LDB_TBL_SCHEMA, LDB_TBL_NAME_DATA ,adrStr, dtStr, dpc->dp[i]->value);
			res = SQLexec(st);
			PQclear(res);
			success += 1;
		}else{
			snprintf(st, CMDBUFFER_SIZE, "UPDATE %s.%s SET \"%s\"='%s' WHERE datetime='%s'", 
                LDB_TBL_SCHEMA, LDB_TBL_NAME_DATA ,adrStr, dpc->dp[i]->value, dtStr );
			res = SQLexec(st);
			PQclear(res);
			success += 1;
		}
	}
  
  //update number of datasets
  snprintf( st, CMDBUFFER_SIZE, "UPDATE %s.%s SET datapakets='%i' WHERE id='%i'", 
            LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICES, dpc->d->datapakets , dpc->d->id);
	res = SQLexec(st);
	PQclear(res);

  snprintf(log, LOG_BUF_LEN, "%i values successfully saved, overall %i datapakets", success, dpc->d->datapakets);
	lielas_log((unsigned char*) log, LOG_LEVEL_DEBUG);

	return 0;
}


/********************************************************************************************************************************
 * 		int handleRtdbgsys(Ldevice* d):
 * 			get rtdbgsys and save it to database
 ********************************************************************************************************************************/

int handleRtdbgsys(Ldevice* d){
  coap_buf *cb;        
  unsigned char buf[DATABUFFER_SIZE] = { 0 };
  char rtdbgsys[(DATABUFFER_SIZE * 2)+1];
  char cmd[CMDBUFFER_SIZE];
  char log[LOG_BUF_LEN];
  int i;
	char st[CMDBUFFER_SIZE];
	PGresult *res;
  time_t rawtime;
  struct tm *now;
	char adrStr[CMDBUFFER_SIZE];
	char dtStr[CMDBUFFER_SIZE];

  cb = coap_create_buf();
  if( cb == NULL){
    snprintf(log, LOG_BUF_LEN, "failed to create coap buffer");
    lielas_log((unsigned char*)log, LOG_LEVEL_ERROR);
    return -1;
  }
  cb->buf = (char*)buf;
  cb->bufSize = DATABUFFER_SIZE;
  
  snprintf(cmd, DATABUFFER_SIZE, "coap://[%s]:5683/rtdbgsys", d->address);        
  coap_send_cmd(cmd, cb, MYCOAP_METHOD_GET, NULL);
  
  if(cb->status != COAP_STATUS_CONTENT){
    lielas_log((unsigned char*) "Status error getting data", LOG_LEVEL_DEBUG);
    return -1;
  }
  
  for(i = 0; i < cb->len; i++){
    snprintf(&rtdbgsys[i * 2], DATABUFFER_SIZE * 2, "%02X", (uint8_t)cb->buf[i]);
  }
  rtdbgsys[i * 2] = 0;
  
  time(&rawtime);
  now = gmtime(&rawtime);
  
  //create column if not existing
	snprintf(adrStr, CMDBUFFER_SIZE, "%s", d->mac);
	if(SQLRowExists(LDB_TBL_NAME_RTDBGSYS, adrStr)){
		snprintf(st, CMDBUFFER_SIZE, "ALTER TABLE %s.%s ADD COLUMN \"%s\" text", LDB_TBL_SCHEMA, LDB_TBL_NAME_RTDBGSYS , adrStr);
		res = SQLexec(st);
		PQclear(res);
	}
  
  // create datetime string
	strftime(dtStr, CMDBUFFER_SIZE, "%Y-%m-%d 00:00:00", now);
    
	//check, if datetime-row exists
	if(SQLCellExists(LDB_TBL_NAME_RTDBGSYS, "datetime", dtStr)){
		//creat new row
		snprintf(st, CMDBUFFER_SIZE, "INSERT INTO %s.%s ( datetime, \"%s\" ) VALUES ( '%s', '%s')", 
               LDB_TBL_SCHEMA, LDB_TBL_NAME_RTDBGSYS ,adrStr, dtStr, rtdbgsys);
		res = SQLexec(st);
		PQclear(res);
	}else{
		snprintf(st, CMDBUFFER_SIZE, "UPDATE %s.%s SET \"%s\"='%s' WHERE datetime='%s'", 
               LDB_TBL_SCHEMA, LDB_TBL_NAME_RTDBGSYS ,adrStr, rtdbgsys, dtStr );
		res = SQLexec(st);
		PQclear(res);
	}
  
  return 0;
}

/********************************************************************************************************************************
 * 		int handleBatmons(Ldevice* d):
 * 			get battery value and save it to database
 ********************************************************************************************************************************/

/********************************************************************************************************************************
 * 		datapaketcontainer CreateDatapaketcontainer()
 ********************************************************************************************************************************/
datapaketcontainer *CreateDatapaketcontainer(){
	datapaketcontainer *dpc = malloc(sizeof(datapaketcontainer));
	return dpc;
}

/********************************************************************************************************************************
 * 		datapaket CreateDatapaket()
 ********************************************************************************************************************************/
datapaket *CreateDatapaket(){
	datapaket *dp = malloc(sizeof(datapaketcontainer));
	if(dp){
		dp->dt = malloc(sizeof(struct tm));
		if(dp->dt == NULL){
			free(dp);
			return NULL;
		}
	}
	return dp;
}

/********************************************************************************************************************************
 * 		void DeleteDatapaket()
 ********************************************************************************************************************************/
void DeleteDatapaket(datapaket *dp){
	if(dp == NULL)
		return;
	free(dp->dt);
	free(dp);
	dp = 0;
}

/********************************************************************************************************************************
 * 		datapaketcontainer DeleteDatapaketcontainer()
 ********************************************************************************************************************************/
void DeleteDatapaketcontainer(datapaketcontainer *dpc){
	int i;
	if(dpc == NULL)
		return;

	for( i = 0; i < dpc->datapakets; i++){
		DeleteDatapaket(dpc->dp[i]);
	}
	free(dpc);
}

uint16_t crc_update(uint16_t crc, uint8_t a){
  int i;
  crc ^= a;
  for(i = 0; i < 8; ++i){
    if(crc & 1)
      crc = (crc >> 1) ^0xA001;
    else
      crc = (crc >> 1);
  }
  return crc;
}

uint16_t get_crc(unsigned char* data, int len){
  int crc  = 0xFFFF;
  int i;
  
  for(i = 0; i < len; i++){
    crc = crc_update(crc, data[i]); 
  }
  return crc;
}






