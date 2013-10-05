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

#include "jsmn/jsmn.h"
#include "settings.h"
#include "log.h"

//#define DEBUG_SETTINGS
void printSetting(char *s, char *v);
int parseAttribute(char *file, char *attr, char *val, int nt, jsmntok_t *tok);

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
	char file[SET_MAX_FILE_LEN];
  long fileSize;
	FILE *fp;
	jsmn_parser json;
	jsmnerr_t r;
	jsmntok_t tokens[MAX_JSON_TOKENS];
  int nextToken;

	fp = fopen("/usr/local/lielas/config.json", "r");
	if( fp == NULL){
		return -1;
	}
  
  fseek(fp, 0, SEEK_END);
  fileSize = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  
  if(fileSize >= SET_MAX_FILE_LEN){
    lielas_log((unsigned char*)"config file too big for buffer", LOG_LEVEL_WARN);
    fclose(fp);
    return -1;
  }
  
  if(fread(file, fileSize, 1, fp) != 1){
    lielas_log((unsigned char*)"error reading config file", LOG_LEVEL_WARN);
    fclose(fp);
    return -1;
  }
  
  jsmn_init(&json);
  nextToken = 0;
	r = jsmn_parse(&json, file, tokens, MAX_JSON_TOKENS);
	if(r != JSMN_SUCCESS && r != JSMN_ERROR_PART){
    lielas_log((unsigned char*)"failed to parse config file", LOG_LEVEL_WARN);
		return -1;
	}
  
  
	while(nextToken < json.toknext && file[tokens[nextToken].start] != 0){
    if(parseAttribute(file, SET_CMD_SQL_USER, sqlUser, nextToken, tokens) == 1)
      nextToken += 2;
    if(parseAttribute(file, SET_CMD_SQL_PASS, sqlPass, nextToken, tokens) == 1)
      nextToken += 2;
    if(parseAttribute(file, SET_CMD_SQL_DB, sqlDb, nextToken, tokens) == 1)
      nextToken += 2;
    if(parseAttribute(file, SET_CMD_SQL_HOST, sqlHost, nextToken, tokens) == 1)
      nextToken += 2;
    if(parseAttribute(file, SET_CMD_SQL_PORT, sqlPort, nextToken, tokens) == 1)
      nextToken += 2;
    nextToken += 1;
  }
  fclose(fp);
	return 0;
}

int parseAttribute(char *file, char *attr, char *val, int nt, jsmntok_t *tok){
  int attrLen;
  char log[LOG_BUF_LEN];
  
  if( tok[nt].type == JSMN_STRING){
    if(!strncmp(&file[tok[nt].start], attr, strlen(attr))){
      nt += 1;
      attrLen = tok[nt].end - tok[nt].start;
      if(attrLen < SET_ATTR_LEN){
        strncpy(val, &file[tok[nt].start], attrLen);
        printSetting(attr, val);
        return 1;
      }else{
        snprintf(log, LOG_BUF_LEN, "attribute %s too big", attr);
        lielas_log((unsigned char*)log, LOG_LEVEL_WARN);
        return -1;
      }
    }
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

void printSetting(char *s, char *v){
  #ifdef DEBUG_SETTINGS
    printf("%s:%s\n", s, v);
  #endif
}






