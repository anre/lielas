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
#include <stdlib.h>
#include <string.h>

#include "ldb.h"
#include "../sql/sql.h"
#include "../log.h"


/********************************************************************************************************************************
 *    int lielas_deleteTable(char* tblname)
 ********************************************************************************************************************************/
int lielas_deleteTable(char* tblname){
  char st[LDB_SQL_BUFFER_SIZE];
  PGresult *res;

  snprintf(st, LDB_SQL_BUFFER_SIZE, "DROP TABLE %s.%s", LDB_TBL_SCHEMA, tblname);
  res = SQLexec(st);
  PQclear(res);

  return(SQLTableExists(tblname));
}

/********************************************************************************************************************************
 *    int lielas_createTables()
 ********************************************************************************************************************************/

int lielas_createTables(){
  int error = 0;

  error -= lielas_createLbusTbl();
  error -= lielas_createUsersTbl();
  //error -= lielas_createDeviceGroupsTbl();
  //error -= lielas_createUserGroupsTbl();
  error -= lielas_createDevicesTbl();
  error -= lielas_createModulsTbl();
  error -= lielas_createChannelsTbl();
  error -= lielas_createDataTbl();
  error -= lielas_createSettingsTbl();
  //error -= lielas_createEventsTbl();
  
  return error;
}

/********************************************************************************************************************************
 *    int lielas_createLbusTbl()
 ********************************************************************************************************************************/

int lielas_createLbusTbl(){
  char st[LDB_SQL_BUFFER_SIZE];
  PGresult *res;

  if(SQLTableExists(LDB_TBL_NAME_LBUS)){
    snprintf(st, LDB_SQL_BUFFER_SIZE, "CREATE TABLE %s.%s %s", LDB_TBL_SCHEMA, LDB_TBL_NAME_LBUS , LDB_TBL_CONTENT_LBUS);
    res = SQLexec(st);
    PQclear(res);
  }
  return 0;
}

/********************************************************************************************************************************
 *    int lielas_createUsersTbl()
 ********************************************************************************************************************************/
int lielas_createUsersTbl(){
  char st[LDB_SQL_BUFFER_SIZE];
  PGresult *res;

  if(SQLTableExists(LDB_TBL_NAME_USERS)){
    snprintf(st, LDB_SQL_BUFFER_SIZE, "CREATE TABLE %s.%s %s", LDB_TBL_SCHEMA, LDB_TBL_NAME_USERS , LDB_TBL_CONTENT_USERS);
    res = SQLexec(st);
    PQclear(res);
    snprintf(st, LDB_SQL_BUFFER_SIZE, "INSERT INTO %s.%s ( id, login, first_name, last_name, timezone, password) VALUES ( 1, 'demo', 'demo', 'user', '0', 'FE01CE2A7FBAC8FAFAED7C982A04E229') ", LDB_TBL_SCHEMA, LDB_TBL_NAME_USERS);
    res = SQLexec(st);
    PQclear(res);

  }
  return 0;
}

/********************************************************************************************************************************
 *    int lielas_createDeviceGroupsTbl()
 ********************************************************************************************************************************/
int lielas_createDeviceGroupsTbl(){
  char st[LDB_SQL_BUFFER_SIZE];
  PGresult *res;

  if(SQLTableExists(LDB_TBL_NAME_DEVICE_GROUPS)){
    snprintf(st, LDB_SQL_BUFFER_SIZE, "CREATE TABLE %s.%s %s", LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICE_GROUPS , LDB_TBL_CONTENT_DEVICE_GROUPS);
    res = SQLexec(st);
    PQclear(res);
  }
  return 0;
}

/********************************************************************************************************************************
 *    int lielas_createUserGroupsTbl()
 ********************************************************************************************************************************/
int lielas_createUserGroupsTbl(){
  char st[LDB_SQL_BUFFER_SIZE];
  PGresult *res;

  if(SQLTableExists(LDB_TBL_NAME_USER_GROUPS)){
    snprintf(st, LDB_SQL_BUFFER_SIZE, "CREATE TABLE %s.%s %s", LDB_TBL_SCHEMA, LDB_TBL_NAME_USER_GROUPS , LDB_TBL_CONTENT_USER_GOUPS);
    res = SQLexec(st);
    PQclear(res);
  }
  return 0;
}

/********************************************************************************************************************************
 *    int lielas_createDevicesTbl()
 ********************************************************************************************************************************/
int lielas_createDevicesTbl(){
  char st[LDB_SQL_BUFFER_SIZE];
  PGresult *res;

  if(SQLTableExists(LDB_TBL_NAME_DEVICES )){
    snprintf(st, LDB_SQL_BUFFER_SIZE, "CREATE TABLE %s.%s %s", LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICES  , LDB_TBL_CONTENT_DEVICES );
    res = SQLexec(st);
    PQclear(res);
  }
  return 0;
}

/********************************************************************************************************************************
 *    int lielas_createModulsTbl()
 ********************************************************************************************************************************/
int lielas_createModulsTbl(){
  char st[LDB_SQL_BUFFER_SIZE];
  PGresult *res;

  if(SQLTableExists(LDB_TBL_NAME_MODULS )){
    snprintf(st, LDB_SQL_BUFFER_SIZE, "CREATE TABLE %s.%s %s", LDB_TBL_SCHEMA, LDB_TBL_NAME_MODULS  , LDB_TBL_CONTENT_MODULS );
    res = SQLexec(st);
    PQclear(res);
  }
  return 0;
}

/********************************************************************************************************************************
 *    int lielas_createChannelsTbl()
 ********************************************************************************************************************************/
int lielas_createChannelsTbl(){
  char st[LDB_SQL_BUFFER_SIZE];
  PGresult *res;

  if(SQLTableExists(LDB_TBL_NAME_CHANNELS )){
    snprintf(st, LDB_SQL_BUFFER_SIZE, "CREATE TABLE %s.%s %s", LDB_TBL_SCHEMA, LDB_TBL_NAME_CHANNELS  , LDB_TBL_CONTENT_CHANNELS );
    res = SQLexec(st);
    PQclear(res);
  }
  return 0;
}

/********************************************************************************************************************************
 *    int lielas_createDataTbl()
 ********************************************************************************************************************************/
int lielas_createDataTbl(){
  char st[LDB_SQL_BUFFER_SIZE];
  PGresult *res;

  if(SQLTableExists(LDB_TBL_NAME_DATA )){
    snprintf(st, LDB_SQL_BUFFER_SIZE, "CREATE TABLE %s.%s %s", LDB_TBL_SCHEMA, LDB_TBL_NAME_DATA, LDB_TBL_CONTENT_DATA );
    res = SQLexec(st);
    PQclear(res);
  }
  return 0;
}

/********************************************************************************************************************************
 *    int lielas_createSettingsTbl()
 ********************************************************************************************************************************/
int lielas_createSettingsTbl(){
  char st[LDB_SQL_BUFFER_SIZE];
  PGresult *res;

  if(SQLTableExists(LDB_TBL_NAME_SETTINGS )){
    snprintf(st, LDB_SQL_BUFFER_SIZE, "CREATE TABLE %s.%s %s", LDB_TBL_SCHEMA, LDB_TBL_NAME_SETTINGS, LDB_TBL_CONTENT_SETTINGS );
    res = SQLexec(st);
    PQclear(res);
    snprintf(st, LDB_SQL_BUFFER_SIZE, "INSERT INTO %s.%s ( name, value) VALUES ( 'DB_VER', '1.0') ", LDB_TBL_SCHEMA, LDB_TBL_NAME_SETTINGS);
    res = SQLexec(st);
    PQclear(res);
    snprintf(st, LDB_SQL_BUFFER_SIZE, "INSERT INTO %s.%s ( name, value) VALUES ( 'PANID', '21554') ", LDB_TBL_SCHEMA, LDB_TBL_NAME_SETTINGS);
    res = SQLexec(st);
    PQclear(res);
    snprintf(st, LDB_SQL_BUFFER_SIZE, "INSERT INTO %s.%s ( name, value) VALUES ( 'REGMODE_LEN', '60') ", LDB_TBL_SCHEMA, LDB_TBL_NAME_SETTINGS);
    res = SQLexec(st);
    PQclear(res);
    snprintf(st, LDB_SQL_BUFFER_SIZE, "INSERT INTO %s.%s ( name, value) VALUES ( 'MAX_REGMODE_LEN', '600') ", LDB_TBL_SCHEMA, LDB_TBL_NAME_SETTINGS);
    res = SQLexec(st);
    PQclear(res);
    snprintf(st, LDB_SQL_BUFFER_SIZE, "INSERT INTO %s.%s ( name, value) VALUES ( 'REG_MINT', '600') ", LDB_TBL_SCHEMA, LDB_TBL_NAME_SETTINGS);
    res = SQLexec(st);
    PQclear(res);
    snprintf(st, LDB_SQL_BUFFER_SIZE, "INSERT INTO %s.%s ( name, value) VALUES ( 'NET_TYPE', 'DHCP') ", LDB_TBL_SCHEMA, LDB_TBL_NAME_SETTINGS);
    res = SQLexec(st);
    PQclear(res);
    snprintf(st, LDB_SQL_BUFFER_SIZE, "INSERT INTO %s.%s ( name, value) VALUES ( 'NET_ADR', '') ", LDB_TBL_SCHEMA, LDB_TBL_NAME_SETTINGS);
    res = SQLexec(st);
    PQclear(res);
    snprintf(st, LDB_SQL_BUFFER_SIZE, "INSERT INTO %s.%s ( name, value) VALUES ( 'NET_MASK', '') ", LDB_TBL_SCHEMA, LDB_TBL_NAME_SETTINGS);
    res = SQLexec(st);
    PQclear(res);
    snprintf(st, LDB_SQL_BUFFER_SIZE, "INSERT INTO %s.%s ( name, value) VALUES ( 'NET_GATEWAY', '') ", LDB_TBL_SCHEMA, LDB_TBL_NAME_SETTINGS);
    res = SQLexec(st);
    PQclear(res);
    snprintf(st, LDB_SQL_BUFFER_SIZE, "INSERT INTO %s.%s ( name, value) VALUES ( 'NET_DNS1', '') ", LDB_TBL_SCHEMA, LDB_TBL_NAME_SETTINGS);
    res = SQLexec(st);
    PQclear(res);
    snprintf(st, LDB_SQL_BUFFER_SIZE, "INSERT INTO %s.%s ( name, value) VALUES ( 'NET_DNS2', '') ", LDB_TBL_SCHEMA, LDB_TBL_NAME_SETTINGS);
    res = SQLexec(st);
    PQclear(res);
    snprintf(st, LDB_SQL_BUFFER_SIZE, "INSERT INTO %s.%s ( name, value) VALUES ( 'NET_NEW_TYPE', 'DHCP') ", LDB_TBL_SCHEMA, LDB_TBL_NAME_SETTINGS);
    res = SQLexec(st);
    PQclear(res);
    snprintf(st, LDB_SQL_BUFFER_SIZE, "INSERT INTO %s.%s ( name, value) VALUES ( 'NET_NEW_ADR', '') ", LDB_TBL_SCHEMA, LDB_TBL_NAME_SETTINGS);
    res = SQLexec(st);
    PQclear(res);
    snprintf(st, LDB_SQL_BUFFER_SIZE, "INSERT INTO %s.%s ( name, value) VALUES ( 'NET_NEW_MASK', '') ", LDB_TBL_SCHEMA, LDB_TBL_NAME_SETTINGS);
    res = SQLexec(st);
    PQclear(res);
    snprintf(st, LDB_SQL_BUFFER_SIZE, "INSERT INTO %s.%s ( name, value) VALUES ( 'NET_NEW_GATEWAY', '') ", LDB_TBL_SCHEMA, LDB_TBL_NAME_SETTINGS);
    res = SQLexec(st);
    PQclear(res);
    snprintf(st, LDB_SQL_BUFFER_SIZE, "INSERT INTO %s.%s ( name, value) VALUES ( 'NET_NEW_DNS1', '') ", LDB_TBL_SCHEMA, LDB_TBL_NAME_SETTINGS);
    res = SQLexec(st);
    PQclear(res);
    snprintf(st, LDB_SQL_BUFFER_SIZE, "INSERT INTO %s.%s ( name, value) VALUES ( 'NET_NEW_DNS2', '') ", LDB_TBL_SCHEMA, LDB_TBL_NAME_SETTINGS);
    res = SQLexec(st);
    PQclear(res);
    snprintf(st, LDB_SQL_BUFFER_SIZE, "INSERT INTO %s.%s ( name, value) VALUES ( 'GW_REG_ADR', '') ", LDB_TBL_SCHEMA, LDB_TBL_NAME_SETTINGS);
    res = SQLexec(st);
    PQclear(res);
    snprintf(st, LDB_SQL_BUFFER_SIZE, "INSERT INTO %s.%s ( name, value) VALUES ( 'GW_NOR_ADR', '') ", LDB_TBL_SCHEMA, LDB_TBL_NAME_SETTINGS);
    res = SQLexec(st);
    PQclear(res);
  }
  return 0;
}

/********************************************************************************************************************************
 *    int lielas_createEventsTbl()
 ********************************************************************************************************************************/
int lielas_createEventsTbl(){
  char st[LDB_SQL_BUFFER_SIZE];
  PGresult *res;

  if(SQLTableExists(LDB_TBL_NAME_EVENTS )){
    snprintf(st, LDB_SQL_BUFFER_SIZE, "CREATE TABLE %s.%s %s", LDB_TBL_SCHEMA, LDB_TBL_NAME_EVENTS, LDB_TBL_CONTENT_EVENTS );
    res = SQLexec(st);
    PQclear(res);
  }
  return 0;
}

/********************************************************************************************************************************
 *    int lielas_getLDBSetting(char* dest, const char* name)
 ********************************************************************************************************************************/
int lielas_getLDBSetting(char* dest, const char* name, int maxLen){
  char st[LDB_SQL_BUFFER_SIZE];
  char log[LOG_BUF_LEN];
  PGresult *res;
  
  snprintf(st, LDB_SQL_BUFFER_SIZE, "SELECT value FROM %s.%s WHERE name='%s'", LDB_TBL_SCHEMA, LDB_TBL_NAME_SETTINGS, name);
  res = SQLexec(st);

	if(PQresultStatus(res) != PGRES_TUPLES_OK){
    snprintf(log, LOG_BUF_LEN, "Failed to set LDB Setting %s", name);
		lielas_log((unsigned char*)log, LOG_LEVEL_WARN);
		PQclear(res);
		return -1;
	}
  
  if(PQntuples(res) != 1){
    snprintf(log, LOG_BUF_LEN, "Failed to set LDB Setting %s, Setting not found or failure in database", name);
		lielas_log((unsigned char*)log, LOG_LEVEL_WARN);
		PQclear(res);
		return -1;
  }
  
  strncpy(dest, PQgetvalue(res, 0, 0), maxLen);
  
  PQclear(res);
  return 0;
}

/********************************************************************************************************************************
 *    int lielas_setLDBSetting(const char* val, const char* name)
 ********************************************************************************************************************************/
int lielas_setLDBSetting(const char* val, const char* name){
  char st[LDB_SQL_BUFFER_SIZE];
  char log[LOG_BUF_LEN];
  PGresult *res;
  
  snprintf(st, LDB_SQL_BUFFER_SIZE, "UPDATE %s.%s SET value='%s' WHERE name='%s'", LDB_TBL_SCHEMA, LDB_TBL_NAME_SETTINGS, val, name);
  res = SQLexec(st);

	if(PQresultStatus(res) != PGRES_COMMAND_OK){
    snprintf(log, LOG_BUF_LEN, "Failed to set LDB Setting %s to %s", name, val);
		lielas_log((unsigned char*)log, LOG_LEVEL_WARN);
		PQclear(res);
		return -1;
	}
  PQclear(res);
  return 0;
}






















