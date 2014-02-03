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

int runmode;
static struct tm *endRegModeTimer;

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
  
  //check if it is time to get data
  //time(&rawtime);
  rawtime = 1391504520;
  now = gmtime(&rawtime);
  
  //debugcode
  endOfTimeslot = now;
  now = malloc(sizeof(struct tm));
  *now = *endOfTimeslot;
  endOfTimeslot = NULL;
  //end of debugcode
  
  //if(now->tm_min != 2){
  //  return;
  //}
  
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
    printf("%i:%i\n", d->datapakets ,datapakets);
    
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
        //time(&rawtime);
        //now = gmtime(&rawtime);
        now->tm_sec += 1;
        mktime(now);
      }
    }
    
    //0 o'clock? test date/time
    if(now->tm_hour == 0){
      DeviceSetDatetime(d);
    }
    
    //check date/time
    //time(&rawtime);
    //now = gmtime(&rawtime);
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
  int16_t val;

  
  lielas_log((unsigned char*) "get device data", LOG_LEVEL_DEBUG);
  
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
  
  while(pos < cb->len){
    
    lwp_compdt_to_struct_tm((unsigned char*)&cb->buf[pos], &dt);
    mktime(&dt);
    pos += 4;
    
    snprintf(log, LOG_BUF_LEN, "found datetime: %x:%x:%x:%x ... ", (unsigned char)cb->buf[pos], (unsigned char)cb->buf[pos+1], (unsigned char)cb->buf[pos+2], (unsigned char)cb->buf[pos+3]);
    strftime(&log[strlen(log)], LOG_BUF_LEN, "%d.%m.%Y %H:%M:%S",&dt);
    lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);
    
    if(testDatetime(&dt)){
      for(cnr = 1; cnr <= d->modul[1]->channels; cnr++){
        dp = CreateDatapaket();
        if(dp == NULL){
          break;
        }
        dp->d = d;
        dp->m = d->modul[1];
        dp->c = d->modul[1]->channel[cnr];
        val = (int16_t)(((uint16_t)cb->buf[pos] << 8) + (uint16_t)cb->buf[pos + 1]);
        snprintf(dp->value, VALUEBUFFER_SIZE, "%i", (int16_t)val);
        pos += 2;
        memcpy(dp->dt, &dt, sizeof(struct tm));
        dpc->dp[dpc->datapakets] = dp;
        dpc->datapakets += 1;
        snprintf(log, LOG_BUF_LEN, "found value: %x:%x ... %s", (unsigned char)cb->buf[pos-2], (unsigned char)cb->buf[pos-1], dp->value);
        lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);
      }
    }

    //uint16_t crc = get_crc((unsigned char*)&cb->buf[pos - 8], 8);
    pos += 2;
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
  
  
	snprintf(cmd, DATABUFFER_SIZE, "coap://[%s]:5683/device", d->address);
	strftime(datetime, DATABUFFER_SIZE, "datetime=%Y.%m.%d-%H:%M:%S", now);
	coap_send_cmd(cmd, cb, MYCOAP_METHOD_PUT, (unsigned char*)datetime);
	sleep(1);

	//get time and check if it is set
/*	buf[0] = 0;
	snprintf(cmd, DATABUFFER_SIZE, "coap://[%s]:5683/device", d->address);
	coap_send_cmd(cmd, cb, MYCOAP_METHOD_GET, NULL);
  
  if(cb->status != COAP_STATUS_CONTENT){
    lielas_log((unsigned char*) "Status error setting datetime\n", LOG_LEVEL_DEBUG);
		return -1;
	}
  
	sleep(1);*/
  
  //TODO parse dt
  
  dt.tm_isdst = 0;
	strptime(recvDt, "%Y.%m.%d-%H:%M:%S", &dt);
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
 * 		int setCycleMode(Ldevice *d, int mode)
 ********************************************************************************************************************************/

int setCycleMode(Ldevice *d, int mode){
	char cmd[CMDBUFFER_SIZE];
	char payload[CMDBUFFER_SIZE];
	char buf[CMDBUFFER_SIZE];
	int tries = 30;
	int i;
	coap_buf *cb = coap_create_buf();

	if(cb == NULL){
		return -1;
	}
  
  if(mode != LWP_CYCLE_MODE_ON && mode != LWP_CYCLE_MODE_OFF){
	  lielas_log((unsigned char*)"error setting cycle mode: unknown cycle mode", LOG_LEVEL_WARN);
    return -1;
  }
  
	cb->buf = buf;
  cb->bufSize = CMDBUFFER_SIZE;
  
	snprintf(cmd, CMDBUFFER_SIZE, "coap://[%s]:5683/network", d->address);
	snprintf(payload, CMDBUFFER_SIZE,"cycling_mode=%i", mode);
  coap_set_retries(1);


	for(i = 0; i < tries; i++){
    lielas_log((unsigned char*)"trying to switch cycle mode", LOG_LEVEL_DEBUG);
		coap_send_cmd(cmd, cb, MYCOAP_METHOD_PUT, (unsigned char*)payload);
		if(cb->status == COAP_STATUS_CONTENT){
      if(mode == LWP_CYCLE_MODE_ON){
        lielas_log((unsigned char*)"successfully turned power cycle on", LOG_LEVEL_DEBUG);
      }else{
        lielas_log((unsigned char*)"successfully turned power cycle off", LOG_LEVEL_DEBUG);
      }
      coap_set_retries(MYCOAP_STD_TRIES);
			return 0;
		}
		buf[0] = 0;
	}
	sleep(2);
  coap_set_retries(MYCOAP_STD_TRIES);
	return -1;
}

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

/********************************************************************************************************************************
 * 		void InitDeviceHandler()
 ********************************************************************************************************************************/
void InitDeviceHandler(){
}

/********************************************************************************************************************************
 *    int lielas_getRunmode()
 ********************************************************************************************************************************/
int lielas_getRunmode(){
  return runmode;
}

/********************************************************************************************************************************
 *    void lielas_setRunmode(int mode)
 ********************************************************************************************************************************/
int lielas_setRunmode(int mode){
  //char cmd[CMDBUFFER_SIZE];
  //char payload[CMDBUFFER_SIZE];
  //coap_buf *cb;
  time_t rawtime;
  struct tm *now;

  //cb = coap_create_buf();

  if(mode != runmode){
    if(mode == RUNMODE_REGISTER){
      lielas_log((unsigned char*) "setting runmode to registration mode", LOG_LEVEL_DEBUG);
      
      if(1){ 

        time(&rawtime);
        now = gmtime(&rawtime);

        if(endRegModeTimer != NULL)
          free(endRegModeTimer);

        endRegModeTimer = malloc(sizeof(struct tm));
        if(endRegModeTimer != NULL){
          runmode = mode;
          memcpy(endRegModeTimer, now, sizeof(struct tm));
          endRegModeTimer->tm_sec += set_getRegModeLen();
          mktime(endRegModeTimer);
        }else{
          lielas_log((unsigned char*) "failed to allocate memory for runtime timer", LOG_LEVEL_WARN);
          return -1;
        }

      }else{
        lielas_log((unsigned char*) "unable to set registration mode", LOG_LEVEL_WARN);
        return -1;
      }
    }else if(mode == RUNMODE_NORMAL){
      lielas_log((unsigned char*) "setting runmode to normal mode", LOG_LEVEL_DEBUG);

      if(1){ 
        runmode = mode;
      }else{
        lielas_log((unsigned char*) "unable to set normal mode", LOG_LEVEL_WARN);
        return -1;
      }
    }
  }
  return 0;
}

/********************************************************************************************************************************
 *    void lielas_runmodeHandler()
 *    switches automatically back to normal mode
 ********************************************************************************************************************************/
void lielas_runmodeHandler(){
  double diff;
  time_t rawtime;
  struct tm *now;

  if(lielas_getRunmode() != RUNMODE_REGISTER)
    return;

  if(endRegModeTimer != NULL){
    time(&rawtime);
    now = gmtime(&rawtime);
    diff = difftime(mktime(now), mktime(endRegModeTimer));
    if(diff < 0 && diff > (0 - set_getMaxRegModeLen())){
      return;

    }
  }

  if(lielas_setRunmode(RUNMODE_NORMAL) != 0){
    lielas_log((unsigned char*)"failed to set runmode to normal mode", LOG_LEVEL_WARN);
  }

}

/********************************************************************************************************************************
 *    struct tm *lielas_getEndRegModeTimer()
 ********************************************************************************************************************************/
struct tm *lielas_getEndRegModeTimer(){
  return endRegModeTimer;
}

uint16_t get_crc(unsigned char* data, int len){
  int crc  = 0;
  int i;
  
  for(i = 0; i < len; i++){
    crc ^= data[i];
    crc  = (crc >> 8) | (crc << 8);
    crc ^= (crc & 0xff00) << 4;
    crc ^= (crc >> 8) >> 4;
    crc ^= (crc & 0xff00) >> 5;   
  }
  return crc;
}






