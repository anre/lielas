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


#define _XOPEN_SOURCE 700

#include "devicehandler.h"
#include "lielas/devicecontainer.h"
#include "lielas/deviceevent.h"
#include "lielas/lielas.h"
#include "coap/libcoap/coap.h"
#include "sql/sql.h"
#include "jsmn/jsmn.h"
#include "coap/mycoap.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

int getDeviceData(Ldevice *d, datapaketcontainer *dpc);
int SaveDataPaketContainerToDatabase(datapaketcontainer* dpc);
datapaketcontainer *CreateDatapaketcontainer();
void DeleteDatapaketcontainer(datapaketcontainer *dpc);
int DeviceSetDatetime(Ldevice *d);
int deactivateCycleMode(Ldevice *d);
datapaket *CreateDatapaket();


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
	deactivateCycleMode(d);
	sleep(2);


	printf("Start device scanning: %s\n", d->address);
	fflush(stdout);

	dpc = CreateDatapaketcontainer();
	if(dpc == NULL)
		return;

	if(!getDeviceData(d, dpc)){

		SaveDataPaketContainerToDatabase(dpc);

	}
	DeleteDatapaketcontainer(dpc);


	DeviceSetDatetime(d);

	printf("End device scanning\n\n\n");
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
	jsmntok_t tokens[MAX_JSON_TOKENS];
	coap_buf *cb;
	int mnr, cnr;
	int dtIsValid;
	int nextToken = 1;
	datapaket *dp;
	PGresult *res;
	jsmn_parser json;
	jsmnerr_t r;

	dpc->datapakets = 0;
	dpc->dec = 0;
	dpc->d = d;

	cb = coap_create_buf();
	if( cb == NULL){
		printf("Error: cannot create coap_buf\n");
		return -1;
	}
	cb->buf = (char*)buf;


	time(&rawtime);
	now = gmtime(&rawtime);
	memcpy(&timeout, now, sizeof(struct tm));
	timeout.tm_sec += MAX_DATA_SEND_TIME;
	mktime(&timeout);




	// get last value in database
//	lastValue.tm_year = 0;
	snprintf(adrStr, DATABUFFER_SIZE, "%s.1.1", d->address);
	if(SQLTableExists("data") == 0){
		snprintf(st, DATABUFFER_SIZE, "SELECT datetime \"%s\" FROM data WHERE \"%s\" NOT LIKE '' ORDER BY datetime DESC LIMIT 1", adrStr, adrStr);
		res = SQLexec(st);
		if(PQntuples(res) == 1){
			strcpy(datetimestr, PQgetvalue(res, 0, 0));
			for(i = 0; i < strlen(datetimestr); i++){
				if(datetimestr[i] == '-'){
					datetimestr[i] = '.';
				}
			}
			snprintf(cmd, DATABUFFER_SIZE, "coap://[%s]:5683/logger/data?datetime=%s", d->address, datetimestr);
			coap_send_cmd(cmd, cb, MYCOAP_METHOD_GET, NULL);
		}else{
			snprintf(cmd, DATABUFFER_SIZE, "coap://[%s]:5683/logger/data?datetime=%s", d->address, GET_FIRST_VALUE_DATE);
			coap_send_cmd(cmd, cb, MYCOAP_METHOD_GET, NULL);
		}
		PQclear(res);
	}else{
		snprintf(st, DATABUFFER_SIZE, "CREATE TABLE data( datetime timestamp NOT NULL, PRIMARY KEY(datetime))");
		res = SQLexec(st);
		PQclear(res);
		if(SQLTableExists("data")){
			return -1;
		}
		snprintf(cmd, DATABUFFER_SIZE, "coap://[%s]:5683/logger/data?datetime=%s", d->address, GET_FIRST_VALUE_DATE);
		coap_send_cmd(cmd, cb, MYCOAP_METHOD_GET, NULL);
	}

	if(cb->status != COAP_STATUS_CONTENT){
		printf("Status error\n");
		return -1;
	}

	jsmn_init(&json);
	r = jsmn_parse(&json, cb->buf, tokens, MAX_JSON_TOKENS);

	if(r != JSMN_SUCCESS && r != JSMN_ERROR_PART){
		printf("JSON Parse error\n");
		return -1;
	}

	while(nextToken < json.toknext && buf[tokens[nextToken].start] != 0){
		//scan date
		strptime((char*)&buf[tokens[nextToken++].start], "%Y.%m.%d %H:%M:%S", &dt);
		dtIsValid = testDatetime(&dt);
		nextToken+=1;

		//scan values
		for(mnr = 1; mnr <= d->moduls; mnr++){
			dpc->m = d->modul[mnr-1];
			for(cnr = 1; cnr <= d->modul[mnr-1]->channels; cnr++){
				if(dtIsValid){
					dp =CreateDatapaket();
					if(dp == NULL)
						break;
					dp->d = d;
					dp->m = d->modul[mnr-1];
					dp->c = d->modul[mnr-1]->channel[cnr-1];

					for(i = 0; i < VALUEBUFFER_SIZE && buf[tokens[nextToken].start + i] != '"' && buf[tokens[nextToken].start + i] != 0; i++){
						dp->value[i] = buf[tokens[nextToken].start + i];
					}
					dp->value[i] = 0;
					memcpy(dp->dt, &dt, sizeof(struct tm));
					dpc->dp[dpc->datapakets] = dp;
					dpc->datapakets += 1;
				}else{
					printf("Date and or Time Error\n");
					fflush(stdout);
				}
				nextToken +=1;
			}
		}
		while(buf[tokens[nextToken].start] == ' '){
			nextToken += 1;
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
	char buf[CMDBUFFER_SIZE];
	coap_buf *cb = coap_create_buf();

	if(cb == NULL){
		return -1;
	}
	cb->buf = buf;

	if(d == NULL){
		return -1;
	}

	time(&rawtime);
	now = gmtime(&rawtime);

	//snprintf(cmd, DATABUFFER_SIZE, "coap://[%s]:5683/logger/logs?datetime=2000.01.01 00:00:00");
	snprintf(cmd, DATABUFFER_SIZE, "coap://[%s]:5683/datetime", d->address);
	strftime(datetime, DATABUFFER_SIZE, "datetime=%Y.%m.%d %H:%M:%S", now);
	coap_send_cmd(cmd, cb, MYCOAP_METHOD_POST, (unsigned char*)datetime);
	sleep(1);

	//get time and check if it is set
	buf[0] = 0;
	snprintf(cmd, DATABUFFER_SIZE, "coap://[%s]:5683/datetime", d->address);
	coap_send_cmd(cmd, cb, MYCOAP_METHOD_GET, NULL);
	sleep(1);
	if(strncmp(buf, "[2.05]", 6)){
		return -1;
	}
	strptime((char*)buf, "[2.05]%Y.%m.%d %H:%M:%S", &dt);

	if(difftime(mktime(now), mktime(&dt)) < MAX_TIME_DIFF){
		return 0;
	}
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
	int success = 0;

	if(SQLTableExists("data")){
		snprintf(st, DATABUFFER_SIZE, "CREATE TABLE data( datetime timestamp NOT NULL, PRIMARY KEY(datetime))");
		res = SQLexec(st);
		PQclear(res);
		if(SQLTableExists("data")){
			return -1;
		}
	}

	// check if Table data exists
	snprintf(st, DATABUFFER_SIZE, "SELECT relname FROM pg_class WHERE relname ='data'");
	res = SQLexec(st);

	if(PQresultStatus(res) != PGRES_TUPLES_OK){
		PQclear(res);
		return -1;
	}
	PQclear(res);

	// create column string
	//snprintf(column, BUFFER_SIZE, "%s.%s.%s", dpc->d->address, dpc->m->address, dpc->c->address);

	for( i = 0; i < dpc->datapakets; i++){

		//create column if not existing
		snprintf(adrStr, DATABUFFER_SIZE, "%s.%s.%s", dpc->dp[i]->d->address, dpc->dp[i]->m->address, dpc->dp[i]->c->address);
		if(SQLRowExists("data", adrStr)){
			snprintf(st, DATABUFFER_SIZE, "ALTER TABLE data ADD COLUMN \"%s\" text", adrStr);
			res = SQLexec(st);
			PQclear(res);
		}

		// create datetime string
		strftime(dtStr, DATABUFFER_SIZE, "%Y-%m-%d %H:%M:%S", dpc->dp[i]->dt);
		//check, if datetime-row exists
		if(SQLCellExists("data", "datetime", dtStr)){
			//creat new row
			snprintf(st, DATABUFFER_SIZE, "INSERT INTO data ( datetime, \"%s\" ) VALUES ( '%s', '%s')", adrStr, dtStr, dpc->dp[i]->value);
			printf("Save data: Address:%s Time:%s Value:%s°C\n", adrStr, dtStr, dpc->dp[i]->value);
			fflush(stdout);
			res = SQLexec(st);
			PQclear(res);
			success += 1;
		}else{
			snprintf(st, DATABUFFER_SIZE, "UPDATE data SET \"%s\"='%s' WHERE datetime='%s'", adrStr, dpc->dp[i]->value, dtStr );
			printf("Save data: Address:%s Time:%s Value:%s°C\n", adrStr, dtStr, dpc->dp[i]->value);
			fflush(stdout);
			res = SQLexec(st);
			PQclear(res);
			success += 1;
		}
	}

	printf("%i Paket(s) successfully safed\n", success);
	fflush(stdout);

	return 0;
}
/********************************************************************************************************************************
 * 		void deactivateCycleMode(Ldevice *d)
 ********************************************************************************************************************************/

int deactivateCycleMode(Ldevice *d){
	char cmd[CMDBUFFER_SIZE];
	char payload[CMDBUFFER_SIZE];
	char datetime[CMDBUFFER_SIZE];
	char buf[CMDBUFFER_SIZE];
	int tries = 30;
	int i;
	time_t rawtime;
	struct tm *now;
	coap_buf *cb = coap_create_buf();

	if(cb == NULL){
		return -1;
	}
	cb->buf = buf;

	snprintf(cmd, CMDBUFFER_SIZE, "coap://[%s]:5683/network/cycling", d->address);
	snprintf(payload, CMDBUFFER_SIZE,"cycling=off");

	for(i = 0; i < tries; i++){
		time(&rawtime);
		now = gmtime(&rawtime);
		strftime(datetime, DATABUFFER_SIZE, "%d.%m.%Y %H:%M:%S", now);
		printf("[%s]turn power cycle off, try %i\n", datetime, i);
		fflush(stdout);

		coap_send_cmd(cmd, cb, MYCOAP_METHOD_POST, (unsigned char*)payload);
		if(cb->status == COAP_STATUS_CONTENT){
			time(&rawtime);
			now = gmtime(&rawtime);
			strftime(datetime, DATABUFFER_SIZE, "%d.%m.%Y %H:%M:%S", now);
			printf("[%s]successfully turned off power cycle\n", datetime);
			fflush(stdout);
			sleep(2);
			return 0;
		}
		buf[0] = 0;
	}
	sleep(2);
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




