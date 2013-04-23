#include <stdio.h>
#include <stdlib.h>

#include "ldb.h"
#include "../sql/sql.h"


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
  error -= lielas_createDeviceGroupsTbl();
  error -= lielas_createUserGroupsTbl();
  error -= lielas_createDevicesTbl();
  error -= lielas_createModulsTbl();
  error -= lielas_createChannelsTbl();
  error -= lielas_createDataTbl();
  error -= lielas_createSettingsTbl();

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
 *    int lielas_createDataTbl()
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
    snprintf(st, LDB_SQL_BUFFER_SIZE, "INSERT INTO %s.%s ( name, value) VALUES ( 'REG_PINT', '2') ", LDB_TBL_SCHEMA, LDB_TBL_NAME_SETTINGS);
    res = SQLexec(st);
    PQclear(res);
    snprintf(st, LDB_SQL_BUFFER_SIZE, "INSERT INTO %s.%s ( name, value) VALUES ( 'REG_AINT', '600') ", LDB_TBL_SCHEMA, LDB_TBL_NAME_SETTINGS);
    res = SQLexec(st);
    PQclear(res);
  }
  return 0;
}




