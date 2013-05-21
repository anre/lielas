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

#include "sql.h"
#include "../log.h"
#include "../settings.h"
#include "libpq-fe.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static PGconn *con = NULL;

/********************************************************************************************************************************
 * 		SQLconnect()
 ********************************************************************************************************************************/


int SQLconnect(){
	char conSt[500] = { 0 };
	strcpy(conSt, "user=");
	strcpy(&conSt[strlen(conSt)], set_getSqlUser());
	strcpy(&conSt[strlen(conSt)], " password=");
	strcpy(&conSt[strlen(conSt)], set_getSqlPass());
	strcpy(&conSt[strlen(conSt)], " dbname=");
	strcpy(&conSt[strlen(conSt)], set_getSqlDb());
	strcpy(&conSt[strlen(conSt)], " host=");
	strcpy(&conSt[strlen(conSt)], set_getSqlHost());
	strcpy(&conSt[strlen(conSt)], " port=");
	strcpy(&conSt[strlen(conSt)], set_getSqlPort());

	con = PQconnectdb(conSt);

	 if(PQstatus(con) != CONNECTION_OK){
		lielas_log((unsigned char*)"Couldn't connect do database using:", LOG_LEVEL_ERROR);
		lielas_log((unsigned char*)conSt, LOG_LEVEL_ERROR);
	    exit(1);
	 }

	atexit(SQLclose);
	return 0;
}

/********************************************************************************************************************************
 * 		SQLclose()
 ********************************************************************************************************************************/

void SQLclose(){
	  if(con != NULL && PQstatus(con) == CONNECTION_OK){
	    PQfinish(con);
	  }
}

/********************************************************************************************************************************
 * 		SQLexec(char *st)
 ********************************************************************************************************************************/

PGresult *SQLexec(char *st){
	PGresult *res;
  char *err;
  char errStr[500];
  
	if(PQstatus(con) != CONNECTION_OK){
		SQLconnect();
	}
	if(PQstatus(con) != CONNECTION_OK){
		return NULL;
	}
	res = PQexec(con, st);
  
  err = PQresultErrorMessage(res);
  if( *err != 0){
    snprintf( errStr, 500, "SQL-error: %s", err);
    lielas_log((unsigned char*) errStr, LOG_LEVEL_DEBUG);
    snprintf( errStr, 500, "executing statement: %s", st);
    lielas_log((unsigned char*) errStr, LOG_LEVEL_DEBUG);
  }
	return res;
}

/********************************************************************************************************************************
 * 		SQLTableExists(char *name)
 ********************************************************************************************************************************/

int SQLTableExists(char *name){
	PGresult *res = 0;
	char buf[SQL_BUFFER_SIZE];
	int rows;
	int i;

	snprintf(buf, SQL_BUFFER_SIZE, "SELECT relname FROM pg_class WHERE relname='%s'", name);
	res = SQLexec(buf);

	if(PQresultStatus(res) != PGRES_TUPLES_OK){
		PQclear(res);
		return -1;
	}

	rows = PQntuples(res);

	for( i = 0; i < rows; i++){
		strcpy(buf, PQgetvalue(res,i, 0));
		if(!strcmp(buf, name)){
			PQclear(res);
			return 0;
		}
	}
	PQclear(res);
	return -1;
}

/********************************************************************************************************************************
 * 		SQLRowExists( char* table, char* row)
 ********************************************************************************************************************************/

int SQLRowExists( char* table, char* row){
	PGresult *res = 0;
	char buf[SQL_BUFFER_SIZE];
	int column;

	snprintf(buf, SQL_BUFFER_SIZE, "SELECT * FROM %s LIMIT 1", table);
	res = SQLexec(buf);

	if(PQresultStatus(res) != PGRES_TUPLES_OK){
		PQclear(res);
		return -1;
	}

	column = PQfnumber(res, row);

	PQclear(res);
	if(column > 0)
		return 0;
	return -1;
}

/********************************************************************************************************************************
 * 		SQLCellExists(char* table, char* column, char* val)
 ********************************************************************************************************************************/

int SQLCellExists(char* table, char* column, char* val){
	PGresult *res = 0;
	char buf[SQL_BUFFER_SIZE];

	snprintf(buf, SQL_BUFFER_SIZE, "SELECT \"%s\" FROM %s WHERE \"%s\"='%s'", column, table, column, val);
	res = SQLexec(buf);

	if(PQresultStatus(res) != PGRES_TUPLES_OK){
		PQclear(res);
		return -1;
	}

	if(PQntuples(res) == 0){
		PQclear(res);
		return -1;
	}

	if(!strcmp(val, PQgetvalue(res, 0, 0))){
		PQclear(res);
		return 0;
	}
	PQclear(res);
	return -1;
}

/********************************************************************************************************************************
 * 		SQLColumnExists(char* table, char* column)
 ********************************************************************************************************************************/

int SQLColumnExists(char* table, char* column){
	PGresult *res = 0;
	char buf[SQL_BUFFER_SIZE];

	snprintf(buf, SQL_BUFFER_SIZE, 
    "SELECT column_name FROM information_schema.columns WHERE table_name='%s' AND column_name='%s'"
    , table, column);

	res = SQLexec(buf);
    
  if(PQresultStatus(res) != PGRES_TUPLES_OK){
    PQclear(res);
    return -1;
	}
	if(PQntuples(res) == 0){
		PQclear(res);
		return -1;
	}
	if(!strcmp(column, PQgetvalue(res, 0, 0))){
		PQclear(res);
		return 0;
	}
  
  return -1;
}


