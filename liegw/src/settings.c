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

#include <stdio.h>
#include <string.h>
#include "settings.h"
#include "log.h"

char sqlUser[SET_ATTR_LEN] = SET_STD_SQL_USER;
char sqlPass[SET_ATTR_LEN] = SET_STD_SQL_PASS;
char sqlDb[SET_ATTR_LEN] 	 = SET_STD_SQL_DB;
char sqlHost[SET_ATTR_LEN] = SET_STD_SQL_HOST;
char sqlPort[SET_ATTR_LEN] = SET_STD_SQL_PORT;

char coapAddr[SET_ATTR_LEN] = SET_STD_COAP_ADDR;
char coapPort[SET_ATTR_LEN] = SET_STD_COAP_PORT;

char gatewaynodeAddr[SET_ATTR_LEN] = SET_STD_GW_ADDR;
char gatewaynodePort[SET_ATTR_LEN] = SET_STD_GW_PORT;

int regmodePanid = SET_STD_REGMODE_PANID;
int normalmodePanid = SET_STD_NORMALMODE_PANID;

int regmodeLen = SET_STD_REGMODE_LEN;
int regmodeMaxLen = SET_STD_MAX_REGMODE_LEN;

int set_load(){
	char line[SET_MAX_LINE_LEN+1];
	FILE *fp;
	int linenr = 0;

	fp = fopen("lielas.conf", "r");
	if( fp == NULL){
		return -1;
	}

	while(fgets(line, SET_MAX_LINE_LEN, fp)){
		linenr += 1;
		if(!strncmp(line, "#", strlen("#"))){	// comment line
			continue;
		}
		if(!strncmp(line, "//", strlen("//"))){	// comment line
			continue;
		}
		if(!strncmp(line, SET_CMD_SQL_USER, strlen(SET_CMD_SQL_USER))){
			strcpy(sqlUser, &line[strlen(SET_CMD_SQL_USER)]);
			continue;
		}
		if(!strncmp(line, SET_CMD_SQL_PASS, strlen(SET_CMD_SQL_PASS))){
			strcpy(sqlPass, &line[strlen(SET_CMD_SQL_PASS)]);
			continue;
		}
		if(!strncmp(line, SET_CMD_SQL_DB, strlen(SET_CMD_SQL_DB))){
			strcpy(sqlDb, &line[strlen(SET_CMD_SQL_DB)]);
			continue;
		}
		if(!strncmp(line, SET_CMD_SQL_HOST, strlen(SET_CMD_SQL_HOST))){
			strcpy(sqlHost, &line[strlen(SET_CMD_SQL_HOST)]);
			continue;
		}
		if(!strncmp(line, SET_CMD_SQL_PORT, strlen(SET_CMD_SQL_PORT))){
			strcpy(sqlPort, &line[strlen(SET_CMD_SQL_PORT)]);
			continue;
		}
		if(!strncmp(line, SET_CMD_COAP_ADDR, strlen(SET_CMD_COAP_ADDR))){
			strcpy(coapAddr, &line[strlen(SET_CMD_COAP_ADDR)]);
			continue;
		}
		if(!strncmp(line, SET_CMD_COAP_PORT, strlen(SET_CMD_COAP_PORT))){
			strcpy(coapPort, &line[strlen(SET_CMD_SQL_PORT)]);
			continue;
		}
		if(!strncmp(line, SET_CMD_GW_ADDR, strlen(SET_CMD_GW_ADDR))){
			strcpy(gatewaynodeAddr, &line[strlen(SET_CMD_SQL_PORT)]);
			continue;
		}
		if(!strncmp(line, SET_CMD_GW_PORT, strlen(SET_CMD_GW_PORT))){
			strcpy(gatewaynodePort, &line[strlen(SET_CMD_GW_PORT)]);
			continue;
		}


		lielas_log((unsigned char*)"Fehler in lielas.conf Zeile %i", linenr);
	}

	return 0;
}

const char *set_getSqlUser(){
	return sqlUser;
}

const char *set_getSqlPass(){
	return sqlPass;
}

const char *set_getSqlDb(){
	return sqlDb;
}

const char *set_getSqlHost(){
	return sqlHost;
}

const char *set_getSqlPort(){
	return sqlPort;
}

const char *set_getCoapAddr(){
	return coapAddr;
}

const char *set_getCoapPort(){
	return coapPort;
}

const char *set_getGatewaynodeAddr(){
	return gatewaynodeAddr;
}

const char *set_getGatewaynodePort(){
	return gatewaynodePort;
}

int set_getStdRegModePanid(){
  return regmodePanid;
}

int set_getStdNormalModePanid(){
  return normalmodePanid;
}

int set_getRegModeLen(){
  return regmodeLen;
}
int set_getMaxRegModeLen(){
  return regmodeMaxLen;
}







