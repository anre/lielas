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


int getDeviceData(Ldevice *d, datapaketcontainer *dpc);
int SaveDataPaketContainerToDatabase(datapaketcontainer* dpc);
datapaketcontainer *CreateDatapaketcontainer();
void DeleteDatapaketcontainer(datapaketcontainer *dpc);
int DeviceSetDatetime(Ldevice *d);
int setCycleMode(Ldevice *d, int mode);
datapaket *CreateDatapaket();
int setLoggerMint(Ldevice *d, Lmodul *m);

int runmode;
static struct tm *endRegModeTimer;

/********************************************************************************************************************************
 * 		void HandleDevices(): handle device events
 ********************************************************************************************************************************/

void HandleDevices(){
	//char uriStr[] = "coap://[aaab::ff:fe00:3]:5863/humidity_sht";

	datapaketcontainer *dpc;
	time_t rawtime;
	struct tm *now;
	struct tm lastPaketTime;
	static struct tm *nextScan;
	double diff;
	static Ldevice *lastDevice;
	Ldevice *d;
	int nextScanInt;
  char log[LOG_BUF_LEN];


	memset(&lastPaketTime, 0, sizeof(struct tm));

	//get systemtime
	time(&rawtime);
	now = gmtime(&rawtime);

	if(nextScan == NULL){	//first call of this function
		nextScan = malloc(sizeof(struct tm));
		if(nextScan == NULL)
			return;
		memcpy(nextScan, now, sizeof(struct tm));
	}else{	// time to get data
		diff = difftime(mktime(now), mktime(nextScan));
		if(diff < 1.0 && diff > (-MAX_GET_DEVICE_DATA_INTERVAL)){
			return;
		}
	}
	if(nextScan == NULL)
		return;

	d = LDCgetFirstDevice();
	if(lastDevice != NULL){
		while(d != lastDevice){
			d = LDCgetNextDevice();
			if(d == NULL){
				break;
			}
		}
		d = LDCgetNextDevice();
		if(d == NULL){
			d = LDCgetFirstDevice();
		}
		lastDevice = d;
	}else{
		lastDevice = d;
	}

	if(LDCgetNextDevice() == NULL){
		nextScanInt =GET_FIRST_DEVICE_DATA_INTERVAL;
	}else{
		nextScanInt =GET_NEXT_DEVICE_DATA_INTERVAL;
	}

	time(&rawtime);
	now = gmtime(&rawtime);
	free(nextScan);
	nextScan = malloc(sizeof(struct tm));
	if(nextScan == NULL)
		return;
	memcpy(nextScan, now, sizeof(struct tm));
	nextScan->tm_sec += nextScanInt;
	mktime(nextScan);
	nextScan->tm_sec = 0;

	if(d == NULL){
		return;
	}

	//turn cycle mode off and sync if out of sync
  //setCycleMode(d, LWP_CYCLE_MODE_OFF);
	sleep(5);

  snprintf(log, LOG_BUF_LEN, "**********\nStart device scanning: %s", d->address);
  lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);

	dpc = CreateDatapaketcontainer();
	if(dpc == NULL)
		return;

	if(!getDeviceData(d, dpc)){
		SaveDataPaketContainerToDatabase(dpc);
	}
	DeleteDatapaketcontainer(dpc);

  //set device date and time
	DeviceSetDatetime(d);
  
  //set interval if changed
  setLoggerMint(d, d->modul[1]);

  //turn cycle mode on again
  //setCycleMode(d, LWP_CYCLE_MODE_ON);
  
  lielas_log((unsigned char*)"End device scanning\n**********", LOG_LEVEL_DEBUG);
	fflush(stdout);

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

/********************************************************************************************************************************
 * 		int getDeviceData(Ldevice *d): get device data and save it to database
 ********************************************************************************************************************************/
int getDeviceData(Ldevice *d, datapaketcontainer *dpc){
	int i;
	time_t rawtime;
	struct tm *now;
	struct tm timeout;
	struct tm dt;
	unsigned char buf[DATABUFFER_SIZE] = { 0 };
	char datetimestr[CMDBUFFER_SIZE];
	char st[CMDBUFFER_SIZE];
	char cmd[CMDBUFFER_SIZE];
	char adrStr[CMDBUFFER_SIZE];
  char log[LOG_BUF_LEN];
	coap_buf *cb;
	int cnr;
	datapaket *dp;
	PGresult *res;
  int pos = 0;
  int eof = 0;
  int datasets = 0;
  uint16_t val;
  

	dpc->datapakets = 0;
	dpc->dec = 0;
	dpc->d = d;


  lielas_log((unsigned char*) "get device data", LOG_LEVEL_DEBUG);

	cb = coap_create_buf();
	if( cb == NULL){
		printf("Error: cannot create coap_buf\n");
		return -1;
	}
	cb->buf = (char*)buf;
  cb->bufSize = DATABUFFER_SIZE;
  
	time(&rawtime);
	now = gmtime(&rawtime);
	memcpy(&timeout, now, sizeof(struct tm));
	timeout.tm_sec += MAX_DATA_SEND_TIME;
	mktime(&timeout);



	// get last value in database
	snprintf(adrStr, DATABUFFER_SIZE, "%s.1.1", d->address);
	if(SQLTableExists(LDB_TBL_NAME_DATA) == 0){    
    //Table exists, check if column exists
    if(SQLColumnExists(LDB_TBL_NAME_DATA, adrStr)){ //column does not exist, get all data
			snprintf(cmd, DATABUFFER_SIZE, "coap://[%s]:5683/database?datetime=%s", d->address, GET_FIRST_VALUE_DATE);
			coap_send_cmd(cmd, cb, MYCOAP_METHOD_GET, NULL);
    }else{ // column exists, get new data
      snprintf(st, DATABUFFER_SIZE, 
                  "SELECT datetime \"%s\" FROM %s.%s WHERE \"%s\" NOT LIKE '' ORDER BY datetime DESC LIMIT 1", 
                  adrStr, LDB_TBL_SCHEMA, LDB_TBL_NAME_DATA , adrStr);
      res = SQLexec(st);
      if(PQntuples(res) == 1){
        strcpy(datetimestr, PQgetvalue(res, 0, 0));
        for(i = 0; i < strlen(datetimestr); i++){
          if(datetimestr[i] == '-'){
            datetimestr[i] = '.';
          }
        }
        snprintf(cmd, DATABUFFER_SIZE, "coap://[%s]:5683/database?datetime=%s", d->address, datetimestr);
        coap_send_cmd(cmd, cb, MYCOAP_METHOD_GET, NULL);
      }else{
        snprintf(cmd, DATABUFFER_SIZE, "coap://[%s]:5683/database?datetime=%s", d->address, GET_FIRST_VALUE_DATE);
        coap_send_cmd(cmd, cb, MYCOAP_METHOD_GET, NULL);
      }
      PQclear(res);
    }
	}else{
		lielas_createDataTbl();
		if(SQLTableExists(LDB_TBL_NAME_DATA)){
			return -1;
		}
		snprintf(cmd, DATABUFFER_SIZE, "coap://[%s]:5683/database?datetime=%s", d->address, GET_FIRST_VALUE_DATE);
		coap_send_cmd(cmd, cb, MYCOAP_METHOD_GET, NULL);
	}

	if(cb->status != COAP_STATUS_CONTENT){
    lielas_log((unsigned char*) "Status error getting data\n", LOG_LEVEL_DEBUG);
		return -1;
	}

  while(pos < cb->len && !eof){

    //parse date
    lwp_compdt_to_struct_tm((unsigned char*)&cb->buf[pos], &dt);
    mktime(&dt);
    
    pos += 4;
      
    //test date
    if(testDatetime(&dt)){
      
      //parse values
      for(cnr = 1; cnr <= d->modul[1]->channels; cnr++){
				dp =CreateDatapaket();
				if(dp == NULL)
					break;
				dp->d = d;
				dp->m = d->modul[1];
				dp->c = d->modul[1]->channel[cnr];
        
        val = ((uint8_t)cb->buf[pos+1]<<8) + (uint8_t)cb->buf[pos];
        snprintf(dp->value, VALUEBUFFER_SIZE, "%u,%u", (val/10), (val%10));
        
        pos += 2;
				memcpy(dp->dt, &dt, sizeof(struct tm));
				dpc->dp[dpc->datapakets] = dp;
				dpc->datapakets += 1;
        
        datasets += 1;
        if(datasets >= MAX_VALUES_IN_PAKET)
          break;
      }
    }else{
      lielas_log((unsigned char*) "error parsing log data, invalid datetime ", LOG_LEVEL_WARN);
      eof = 1;
      break;
    }
 
  }
  snprintf(log, LOG_BUF_LEN, "received %i values", datasets);
  lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);
  
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
	strftime(datetime, DATABUFFER_SIZE, "datetime=%Y.%m.%d %H:%M:%S", now);
	coap_send_cmd(cmd, cb, MYCOAP_METHOD_PUT, (unsigned char*)datetime);
	sleep(1);

	//get time and check if it is set
	buf[0] = 0;
	snprintf(cmd, DATABUFFER_SIZE, "coap://[%s]:5683/device", d->address);
	coap_send_cmd(cmd, cb, MYCOAP_METHOD_GET, NULL);
  
  if(cb->status != COAP_STATUS_CONTENT){
    lielas_log((unsigned char*) "Status error setting datetime\n", LOG_LEVEL_DEBUG);
		return -1;
	}
  
	sleep(1);
  
  if(lwp_get_attr_value(cb->buf, &d->wkc.device, LWP_ATTR_DEVICE_DATETIME, recvDt, DEVICE_MAX_STR_LEN)){
    lielas_log((unsigned char*)"failed to parse /devcice/datetime", LOG_LEVEL_WARN);
    return -1;
  }
  dt.tm_isdst = 0;
	strptime(recvDt, "%Y.%m.%d %H:%M:%S", &dt);
  
	time(&rawtime);
	now = gmtime(&rawtime);
  
	if(difftime(mktime(now), mktime(&dt)) < MAX_TIME_DIFF){
		return 0;
	}
  
  lielas_log((unsigned char*)"setting time: time difference too big", LOG_LEVEL_WARN);
  snprintf(log, CMDBUFFER_SIZE, "server time: ");
	strftime(&log[strlen(log)], CMDBUFFER_SIZE - strlen(log), "%d.%m.%Y %H:%M:%S", now);
  snprintf(&log[strlen(log)], CMDBUFFER_SIZE - strlen(log), " received time: ");
	strftime(&log[strlen(log)], CMDBUFFER_SIZE - strlen(log), "%d.%m.%Y %H:%M:%S",&dt);
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
		snprintf(adrStr, CMDBUFFER_SIZE, "%s.%s.%s", dpc->dp[i]->d->address, dpc->dp[i]->m->address, dpc->dp[i]->c->address);
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
      //snprintf(log, LOG_BUF_LEN, "Saved data: Address:%s Time:%s Value:%s°C", adrStr, dtStr, dpc->dp[i]->value);
			//lielas_log((unsigned char*) log, LOG_LEVEL_DEBUG);
			res = SQLexec(st);
			PQclear(res);
			success += 1;
		}else{
			snprintf(st, CMDBUFFER_SIZE, "UPDATE %s.%s SET \"%s\"='%s' WHERE datetime='%s'", 
                LDB_TBL_SCHEMA, LDB_TBL_NAME_DATA ,adrStr, dpc->dp[i]->value, dtStr );
      //snprintf(log, LOG_BUF_LEN, "Saved data: Address:%s Time:%s Value:%s°C", adrStr, dtStr, dpc->dp[i]->value);
			//lielas_log((unsigned char*) log, LOG_LEVEL_DEBUG);
			res = SQLexec(st);
			PQclear(res);
			success += 1;
		}
	}

  snprintf(log, LOG_BUF_LEN, "%i values successfully saved", success);
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
 * 		int setLoggerMint(Ldevice *d, Lmodul *m)
 ********************************************************************************************************************************/
int setLoggerMint(Ldevice *d, Lmodul *m){
	char cmd[CMDBUFFER_SIZE];
	char payload[CMDBUFFER_SIZE];
	char buf[CMDBUFFER_SIZE];  
  char recvMint[CMDBUFFER_SIZE];
  char log[LOG_BUF_LEN];
	coap_buf *cb = coap_create_buf();
  
  if(cb == NULL){
		return -1;
	}
  
	cb->buf = buf;
  cb->bufSize = CMDBUFFER_SIZE;
  
  //get logger resource
 	snprintf(cmd, CMDBUFFER_SIZE, "coap://[%s]:5683/logger", d->address);
	coap_send_cmd(cmd, cb, MYCOAP_METHOD_GET, NULL);
  
  if(cb->status != COAP_STATUS_CONTENT){
    lielas_log((unsigned char*) "Status error getting resource /logger", LOG_LEVEL_WARN);
    cb->buf = NULL;
    coap_delete_buf(cb);
		return -1;
	}
  
  if(lwp_get_attr_value(cb->buf, &d->wkc.logger, LWP_ATTR_LOGGER_INTERVAL, recvMint, CMDBUFFER_SIZE)){
    lielas_log((unsigned char*)"setLoggerMint: failed to parse /logger/interval", LOG_LEVEL_WARN);
    cb->buf = NULL;
    coap_delete_buf(cb);
    return -1;
  }
  
  //test if interval was changed
  if(strcmp(m->mint, recvMint)){
    snprintf(log, LOG_BUF_LEN, "setLoggerMint: interval changed, setting interval to %s", m->mint);
    lielas_log((unsigned char*)log, LOG_LEVEL_WARN);
    snprintf(payload, CMDBUFFER_SIZE, "interval=%s", m->mint);
    coap_send_cmd(cmd, cb, MYCOAP_METHOD_PUT, (unsigned char*)payload);
    
    if(cb->status != COAP_STATUS_CONTENT){
      lielas_log((unsigned char*) "setLoggerMint: failed to set interval", LOG_LEVEL_WARN);
      cb->buf = NULL;
      coap_delete_buf(cb);
      return -1;
    }
  }
  cb->buf = NULL;
  coap_delete_buf(cb);
  return 0;
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

	Ldevice *d = LDCgetFirstDevice();

	if(d == NULL)
		return;
	do{
		if(d->registered == DEVICE_REGISTERED){
			if(d ->hasLogger == DEVICE_HAS_LOGGER){
			//	LcreateEvent(d, EVENTTYPE_READ_LOGGER);
			}
		}
		d = LDCgetNextDevice();
	}while(d != NULL);
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
  char cmd[CMDBUFFER_SIZE];
  char payload[CMDBUFFER_SIZE];
  coap_buf *cb;
  time_t rawtime;
  struct tm *now;

  cb = coap_create_buf();

  if(mode != runmode){
    if(mode == RUNMODE_REGISTER){
      lielas_log((unsigned char*) "setting runmode to registration mode", LOG_LEVEL_DEBUG);
      snprintf(cmd, CMDBUFFER_SIZE, "coap://[%s]:%s/network", set_getGatewaynodeAddr(), set_getGatewaynodePort());
      snprintf(payload, CMDBUFFER_SIZE,"panid=%d", set_getStdRegModePanid());

      coap_send_cmd(cmd, cb, MYCOAP_METHOD_PUT, (unsigned char*)payload);
      if(1){ //DEBUG
      //if(cb->status == COAP_STATUS_CONTENT){

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
      snprintf(cmd, CMDBUFFER_SIZE, "coap://[%s]:%s/network", set_getGatewaynodeAddr(), set_getGatewaynodePort());
      snprintf(payload, CMDBUFFER_SIZE,"panid=%d", set_getStdNormalModePanid());

      coap_send_cmd(cmd, cb, MYCOAP_METHOD_PUT, (unsigned char*)payload);
      if(1){ //DEBUG
      //if(cb->status == COAP_STATUS_CONTENT){
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




