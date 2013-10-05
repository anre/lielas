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

//#define DEBUG_LBUS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "lbus.h"
#include "ldb.h"
#include "device.h"
#include "devicecontainer.h"
#include "../devicehandler.h"
#include "../log.h"
#include "../sql/sql.h"
#include "../jsmn/jsmn.h"
#include "../rtc/rtc.h"

static Lbuscontainer *lbuscon;
static Lbuscontainer *lbusconPtr;

static int handleDelCmd(Lbuscmd *cmd);
static int handleChgCmd(Lbuscmd *cmd);
static int handleLoginCmd(Lbuscmd *cmd);
static int parsePayload(char *payload, jsmn_parser *json, jsmntok_t *tokens, int maxtokens);
static int getNewId();
static int saveCmd(Lbuscmd *cmd);
static int setCmdHandled(Lbuscmd *cmd);

static int changeDevice(Lbuscmd *cmd, int tok, jsmntok_t *tokens, int maxTokens);
static int changeNetType(Lbuscmd *cmd, int tok, jsmntok_t *tokens, int maxTokens);
static int changeRtcDateTime(Lbuscmd *cmd, int tok, jsmntok_t *tokens, int maxTokens);

//debug functions
void printLbusCmd(Lbuscmd* cmd);
void printLbusCmdCon();

// netTypeChanged
// set to 1 if changing netType and to 0 after successfully login
// if after LBUS_NET_RELOGIN_TIME netTypeChanged is 1, reset net settings
static int netTypeChanged;

pthread_mutex_t mutex;

/********************************************************************************************************************************
 * 		void lbus_init()
 ********************************************************************************************************************************/
int lbus_init(){
  pthread_mutexattr_t mutexAttr = {};
  
  lbuscon = malloc(sizeof(Lbuscontainer));
	if(lbuscon == NULL){
		lielas_log((unsigned char*)"Couldn't allocate memory for lbus container", LOG_LEVEL_ERROR);
		return -1;
	}
  
  lbuscon->llbc = NULL;
  lbuscon->nlbc = NULL;
  lbuscon->lcmd = NULL;
  lbusconPtr = lbuscon;
  
  netTypeChanged = 1;
  
  pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_RECURSIVE);
  if(pthread_mutex_init(&mutex, &mutexAttr) != 0){
		lielas_log((unsigned char*)"Couldn't initialize mutex for lbus container", LOG_LEVEL_ERROR);
		return -1;
  }

  return 0;
}

/********************************************************************************************************************************
 *    int lbus_load()
 ********************************************************************************************************************************/
int lbus_load(){
  PGresult *res;
  PGresult *resDel;
  char st[SQL_STATEMENT_BUF_SIZE];
  char stDel[SQL_STATEMENT_BUF_SIZE];
  char dt[50];
  int rows;
  int i;
  Lbuscmd *lbuscmd;
  long id;
  char log[LOG_BUF_LEN];
  
  pthread_mutex_lock(&mutex);
  
  snprintf(st, SQL_STATEMENT_BUF_SIZE, "SELECT id, tmrecv, usr, address, cmd, payload, tmnexthandle FROM %s.%s WHERE handled='false'", LDB_TBL_SCHEMA, LDB_TBL_NAME_LBUS);
  res = SQLexec(st);
  
  if(PQresultStatus(res) != PGRES_TUPLES_OK){
		lielas_log((unsigned char*)"Failed to get lbus commands or no commands", LOG_LEVEL_WARN);
		PQclear(res);
    pthread_mutex_unlock(&mutex);
		return -1;
	}
  
 	rows = PQntuples(res);
  
  for(i = 0 ;i < rows; i++){
    id = atoi(PQgetvalue(res, i, 0));
    
    if(id <= 0){
      lielas_log((unsigned char*)"Error parsing lbus command: failed to parse id", LOG_LEVEL_WARN);
      PQclear(res);
      pthread_mutex_unlock(&mutex);
      return -1;
    }
    
    lbuscmd = lbus_createCmd(id);
    
    if(lbuscmd == NULL){
      lielas_log((unsigned char*)"Error parsing lbus command: failed to allocate space for lbus command", LOG_LEVEL_WARN);
      PQclear(res);
      pthread_mutex_unlock(&mutex);
      return -1;
    }
    
    strcpy(dt, PQgetvalue(res,  i, 1));
    strptime(dt, "%Y-%m-%d %H:%M:%S", &lbuscmd->tmrecv);
    strcpy(lbuscmd->user, PQgetvalue(res,  i, 2));
    strcpy(lbuscmd->address, PQgetvalue(res,  i, 3));
    strcpy(lbuscmd->cmd, PQgetvalue(res,  i, 4));
    strcpy(lbuscmd->payload, PQgetvalue(res, i, 5));
    strcpy(dt, PQgetvalue(res,  i, 6));
    if(strlen(dt) == 0){
      lbuscmd->tmnexthandle = 0;
    }else{
      lbuscmd->tmnexthandle = malloc(sizeof(struct tm));
      if(lbuscmd->tmnexthandle == 0){
        lielas_log((unsigned char*)"Error parsing lbus command: failed to allocate space for lbus tmnexthandle", LOG_LEVEL_WARN);
        PQclear(res);
        pthread_mutex_unlock(&mutex);
        return -1;
      }
      strptime(dt, "%Y-%m-%d %H:%M:%S", lbuscmd->tmnexthandle);
    }
    lbuscmd->handled = 0;
    
    if(lbuscmd->cmd[0] == 0){
      //empty entry, delete it  
      snprintf(stDel, SQL_STATEMENT_BUF_SIZE, "DELETE FROM %s.%s WHERE id=%li", LDB_TBL_SCHEMA, LDB_TBL_NAME_LBUS, id);
      resDel = SQLexec(stDel);
      PQclear(resDel);
      
    }else{
      lbus_add(lbuscmd, 0);
    }
    free(lbuscmd);
  }
  
  printLbusCmdCon();
  
  snprintf(log, LOG_BUF_LEN, "%i lbus commands loaded", i);
  lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);
  
  PQclear(res);
  pthread_mutex_unlock(&mutex);
  
  return 0;
}

/********************************************************************************************************************************
 *    int lbus_add(Lbuscmd *cmd, int saveToDb)
 ********************************************************************************************************************************/

int lbus_add(Lbuscmd *cmd, int saveToDb){
  pthread_mutex_lock(&mutex);
  char log[LOG_BUF_LEN];
  
  if(cmd == NULL){
    snprintf(log, LOG_BUF_LEN, "invalid lbus command pointer");
    lielas_log((unsigned char*)log, LOG_LEVEL_ERROR);
    return -1;
  }
  
  Lbuscmd *newCmd = malloc(sizeof(Lbuscmd));
  
  snprintf(log, LOG_BUF_LEN, "adding new cmd: cmd=%s", cmd->cmd);
  lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);
  
  if(newCmd == NULL){
		lielas_log((unsigned char*)"Couldn't allocate memory for lbus command", LOG_LEVEL_WARN);
    pthread_mutex_unlock(&mutex);
    return -1;
  }
  lbus_getFirstCmd();
  if(lbuscon == NULL){
		lielas_log((unsigned char*)"lbus container not initialized", LOG_LEVEL_ERROR);
    free(newCmd);
    pthread_mutex_unlock(&mutex);
    return -1;
  }else if(lbusconPtr->nlbc == NULL&& lbusconPtr->lcmd == NULL){ // first entry
    lbusconPtr->lcmd = newCmd;
    *newCmd = *cmd;

    // save to database
    if(saveToDb){
      if(saveCmd(newCmd)){
        free(newCmd);
        pthread_mutex_unlock(&mutex);
        return -1;
      }
    }

    pthread_mutex_unlock(&mutex);
    snprintf(log, LOG_BUF_LEN, "Successfully added lbus command with id %li", cmd->id);
		lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);
    return 0;

  }else{
    
    //search last entry
    while(lbusconPtr->nlbc != NULL){
      lbus_getNextCmd();
    }
    
    //create new lbus container
    Lbuscontainer *newlbuscon= malloc(sizeof(Lbuscontainer));  
    if(newlbuscon == NULL){
      lielas_log((unsigned char*)"Couldn't allocate memory for lbus container", LOG_LEVEL_ERROR);
      free(newCmd);
      pthread_mutex_unlock(&mutex);
      return -1;
    } 
    
    //register new lbus container
    lbusconPtr->nlbc = newlbuscon;
    newlbuscon->llbc = lbusconPtr;
    newlbuscon->nlbc = NULL;
    lbusconPtr = newlbuscon;
    
    //register cmd
    lbusconPtr->lcmd = newCmd;
    *newCmd = *cmd;
  }

  // save to database
  if(saveToDb){
    if(saveCmd(newCmd)){
      free(newCmd);
      pthread_mutex_unlock(&mutex);
      return -1;
    }
  }

  pthread_mutex_unlock(&mutex);
  snprintf(log, LOG_BUF_LEN, "Successfully added lbus command with id %li", cmd->id);
	lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);
  return 0;
}

/********************************************************************************************************************************
 * 		void lbus_remove(Lbuscmd *cmd)
 ********************************************************************************************************************************/
void lbus_remove(Lbuscmd *cmd){
  Lbuscontainer *con;
  char log[LOG_BUF_LEN];
  
  pthread_mutex_lock(&mutex);
  

  snprintf(log, LOG_BUF_LEN, "removing lbus cmd %li from lbus container", cmd->id);
  lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);
  if(cmd == NULL){
    lielas_log((unsigned char*)"can't remove cmd, cmd is null", LOG_LEVEL_WARN);
    pthread_mutex_unlock(&mutex);
    return;
  }

  if(lbuscon == NULL){
      lielas_log((unsigned char*)"can't remove cmd, lbus container not initialized", LOG_LEVEL_WARN);
      pthread_mutex_unlock(&mutex);
      return;
  }

  con = lbuscon;

  while(con != NULL && con->lcmd->id != cmd->id){
    con = con->nlbc;
  }

  if(con == NULL){
    lielas_log((unsigned char*)"can't remove cmd, cmd not found in container", LOG_LEVEL_WARN);
    pthread_mutex_unlock(&mutex);
    return;
  }

  if(con->nlbc == NULL && con->llbc == NULL){
    //only one item in container
    lbus_deleteCmd(con->lcmd);
    con->lcmd = 0;
  }else if(con->llbc == NULL){
    //delete first item in container
    free(lbuscon);
    lbuscon = con->nlbc;
    lbuscon->llbc = NULL;
  }else{
    con->llbc->nlbc = con->nlbc;
    if( con->nlbc != NULL){
      con->nlbc->llbc = con->llbc;
    }
    lbus_deleteCmd(con->lcmd);
    free(con);
  }
  
  
  lielas_log((unsigned char*)"lbus cmd removed", LOG_LEVEL_DEBUG);
  pthread_mutex_unlock(&mutex);

}

/********************************************************************************************************************************
 * 		Lbuscmd *lbus_createCmd()
 ********************************************************************************************************************************/
Lbuscmd *lbus_createCmd(int id){
  
  pthread_mutex_lock(&mutex);
  
	Lbuscmd *cmd = malloc(sizeof(Lbuscmd));
	if(cmd == NULL){
		return NULL;
	}
  
  cmd->id = id;
  if(cmd->id == 0){
    //get new id from database

    cmd->id = getNewId();
    if(cmd->id == 0){
      lielas_log((unsigned char*)"Failed to get new lbus cmd id", LOG_LEVEL_WARN);
      free(cmd);
      pthread_mutex_unlock(&mutex);
      return NULL;
    }
  }

	cmd->handled = 0;
	cmd->address[0] = 0;
	cmd->cmd[0] = 0;
	cmd->payload[0] = 0;
  cmd->tmnexthandle = 0;
  pthread_mutex_unlock(&mutex);

	return cmd;
}

/********************************************************************************************************************************
 *    void lbus_deleteCmd(Lbuscmd *cmd)
 ********************************************************************************************************************************/
void lbus_deleteCmd(Lbuscmd *cmd){
  
  if(cmd->tmnexthandle != NULL)
    free(cmd->tmnexthandle);
  
  free(cmd);
  cmd = 0;
}


/********************************************************************************************************************************
 * 		Lbuscmd *lbus_getFirstCmd()
 ********************************************************************************************************************************/
Lbuscmd *lbus_getFirstCmd(){
  pthread_mutex_lock(&mutex);
  
  if(lbuscon == NULL){
    pthread_mutex_unlock(&mutex);
	  return NULL;
  }
    
  lbusconPtr = lbuscon;
  
  pthread_mutex_unlock(&mutex);
  return lbusconPtr->lcmd;
}

/********************************************************************************************************************************
 * 		Lbuscmd *lbus_getNextCmd()
 ********************************************************************************************************************************/
Lbuscmd *lbus_getNextCmd(){
  pthread_mutex_lock(&mutex);
  
  if(lbuscon == NULL){
    pthread_mutex_unlock(&mutex);
    return NULL;
  }
  
  if(lbusconPtr != NULL){
    lbusconPtr = lbusconPtr->nlbc;
  }
  
  if(lbusconPtr == NULL){
    pthread_mutex_unlock(&mutex);
	  return NULL;
  }
  
  pthread_mutex_unlock(&mutex);
  return lbusconPtr->lcmd;
}

/********************************************************************************************************************************
 *    void lbus_handler()
 ********************************************************************************************************************************/
void lbus_handler(){
  static Lbuscmd *cmd;
  pthread_mutex_lock(&mutex);
	time_t rawtime;
	struct tm *now;

  // look if unhandled cmd are in the buffer
  if(cmd == NULL){
    cmd = lbus_getFirstCmd();
  }else{
    cmd = lbus_getNextCmd();
  }
  
  if(cmd == NULL){
    pthread_mutex_unlock(&mutex);
    return;
  }

	time(&rawtime);
	now = gmtime(&rawtime);

  if(cmd->tmnexthandle == NULL || (difftime(mktime(cmd->tmnexthandle), mktime(now))< 1.0)){
    
    //unhandled cmd found
    if(!strcmp(cmd->cmd, LBUS_CMD_DEL )){
      handleDelCmd(cmd);
    }else if(!strcmp(cmd->cmd, LBUS_CMD_CHG )){
      handleChgCmd(cmd);
    }else if(!strcmp(cmd->cmd, LBUS_CMD_LOGIN )){
      handleLoginCmd(cmd);
    }else{
      lielas_log((unsigned char*)"unknown lbus command", LOG_LEVEL_WARN);
      setCmdHandled(cmd);
    }
  }

  pthread_mutex_unlock(&mutex);
}

/********************************************************************************************************************************
 *    void lbus_printCmd(char *s, int maxLen, Lbuscmd *cmd)
 ********************************************************************************************************************************/
int lbus_printCmd(char *s, int maxLen, Lbuscmd *cmd){
  int len = 0;

  if(cmd == NULL){
    s[0] = 0;
    return 0;
  }

  len += snprintf((char*)&s[len], (maxLen-len), "{\n");
  len += snprintf((char*)&s[len], (maxLen-len), "  \"id\":\"%li\",\n", cmd->id);
  len += strftime((char*)&s[len], (maxLen-len), "  \"tmrecv\":\"%d.%m.%Y %H:%M:%S\",\n", &cmd->tmrecv);
  len += snprintf((char*)&s[len], (maxLen-len), "  \"address\":\"%s\",\n", cmd->address);
  len += snprintf((char*)&s[len], (maxLen-len), "  \"user\":\"%s\",\n", cmd->user);
  len += snprintf((char*)&s[len], (maxLen-len), "  \"cmd\":\"%s\",\n", cmd->cmd);
  len += snprintf((char*)&s[len], (maxLen-len), "  \"payload\":%s\n", cmd->payload);
  len += snprintf((char*)&s[len], (maxLen-len), "  \"handled\":\"%s\",\n", (cmd->handled)?"true":"false");
  if(cmd->handled){
    len += strftime((char*)&s[len], (maxLen-len), "  \"tmhandled\":\"%d.%m.%Y %H:%M:%S\",\n", &cmd->tmhandled);
  }else{
    len += snprintf((char*)&s[len], (maxLen-len), "  \"tmhandled\":\"\",\n");
  }
  len += snprintf((char*)&s[len], (maxLen-len), "}\n");

  return len;
}

/********************************************************************************************************************************
 * 		static int handleLoginCmd(Lbuscmd *cmd)
 ********************************************************************************************************************************/
static int handleLoginCmd(Lbuscmd *cmd){
  Lbuscmd *netcmd;
  
  pthread_mutex_lock(&mutex);
  setCmdHandled(cmd);
  netTypeChanged = 0;
  
  // look if unhandled chng network settings cmds are in the buffer
  netcmd = lbus_getFirstCmd();
  
  while( netcmd != NULL){
    if(!strcmp(netcmd->cmd, LBUS_CMD_CHG )){
      handleChgCmd(netcmd);
    }
    netcmd = lbus_getNextCmd();
  }
  
  pthread_mutex_unlock(&mutex);
  return 0;
}


/********************************************************************************************************************************
 * 		static int handleDelCmd(Lbuscmd *cmd)
 ********************************************************************************************************************************/
static int handleDelCmd(Lbuscmd *cmd){
  jsmntok_t tokens[MAX_JSON_TOKENS];
  jsmn_parser json;
  char errStr[LBUF_MAX_ERR_STR_SIZE];
  char idStr[LBUF_MAX_ERR_STR_SIZE];
  char log[LOG_BUF_LEN];
  int tok;
  int parseError = 0;
  int id;
  PGresult *res;
  char st[SQL_STATEMENT_BUF_SIZE];
  int i, j;
  
  
  pthread_mutex_lock(&mutex);

  //parse address
  if(!strcmp(cmd->address, LBUS_ADDRESS_LIEGW)){
    
    
    //address: liegw
    if(parsePayload(cmd->payload, &json, tokens, MAX_JSON_TOKENS) != 0){
      lielas_log((unsigned char*)"JSON parse error parsing lbus payload:", LOG_LEVEL_WARN);
      lbus_printCmd(errStr, LBUF_MAX_ERR_STR_SIZE, cmd);
      lielas_log((unsigned char*)errStr, LOG_LEVEL_WARN);
    }
    
    for(tok = 0; tok< json.toknext && !parseError; tok++){
      if(tokens[tok].type == JSMN_STRING){
        if(strncmp((char*)&cmd->payload[tokens[tok].start], LBUS_TOK_DEVICE, strlen(LBUS_TOK_DEVICE)) == 0){
          tok+=1;
          strncpy(idStr, (char*)&cmd->payload[tokens[tok].start], tokens[tok].end - tokens[tok].start);
          id = atoi(idStr);
          
          if(id <= 0){
            lielas_log((unsigned char*)"Error parsing lbus command: delete device: error parsing device id", LOG_LEVEL_WARN);
            pthread_mutex_unlock(&mutex);
            return 0;
          }
          
          //delete device
          Ldevice *d;
          LDCgetDeviceById(id, &d);
          
          if(d != NULL){
          
            snprintf(log, LOG_BUF_LEN, "Device %s deleted", d->mac);
            
            for(i = 1; i <= d->moduls; i++){
              for(j = 1; j <= d->modul[i]->channels; j++){
                snprintf(st, SQL_STATEMENT_BUF_SIZE, "DELETE FROM %s.%s WHERE id=%i", LDB_TBL_SCHEMA, LDB_TBL_NAME_CHANNELS, d->modul[i]->channel[j]->id);
                res = SQLexec(st);
                PQclear(res);
                snprintf(log, LOG_BUF_LEN, "deleted channel %s.%s.%s", d->mac, d->modul[i]->address, d->modul[i]->channel[j]->address);
                lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);
              }
              snprintf(st, SQL_STATEMENT_BUF_SIZE, "DELETE FROM %s.%s WHERE id=%i", LDB_TBL_SCHEMA, LDB_TBL_NAME_MODULS, d->modul[i]->id);
              res = SQLexec(st);
              PQclear(res);
              snprintf(log, LOG_BUF_LEN, "deleted modul %s.%s", d->mac, d->modul[i]->address);
              lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);
            }
            snprintf(st, SQL_STATEMENT_BUF_SIZE, "DELETE FROM %s.%s WHERE id=%i", LDB_TBL_SCHEMA, LDB_TBL_NAME_DEVICES, d->id);
            res = SQLexec(st);
            PQclear(res);
            snprintf(log, LOG_BUF_LEN, "deleted device %s", d->mac);
            lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);
            
            LDCremove(d);
            setCmdHandled(cmd);
            lielas_log((unsigned char*) log, LOG_LEVEL_DEBUG);
          }else{
            lielas_log((unsigned char*) "can't delete device, device not found", LOG_LEVEL_DEBUG);
            setCmdHandled(cmd);
          }
        }
      } // if token == JSMN_STRING
    } // for every token
  }

  pthread_mutex_unlock(&mutex);
  return 0;
}

/********************************************************************************************************************************
 *    static int handleChgCmd(Lbuscmd *cmd)
 ********************************************************************************************************************************/
static int handleChgCmd(Lbuscmd *cmd){
  jsmntok_t tokens[MAX_JSON_TOKENS];
  jsmn_parser json;
  char errStr[LBUF_MAX_ERR_STR_SIZE];
  int tok;
  int parseError = 0;

  pthread_mutex_lock(&mutex);
  
  //parse address
  if(!strcmp(cmd->address, LBUS_ADDRESS_LIEGW)){
    //address: liegw
    // parse payload
    if(parsePayload(cmd->payload, &json, tokens, MAX_JSON_TOKENS) != 0){
      lielas_log((unsigned char*)"JSON parse error parsing lbus payload:", LOG_LEVEL_WARN);
      lbus_printCmd(errStr, LBUF_MAX_ERR_STR_SIZE, cmd);
      lielas_log((unsigned char*)errStr, LOG_LEVEL_WARN);
      pthread_mutex_unlock(&mutex);
      return -1;
    }

    for(tok = 0; tok< json.toknext && !parseError; tok++){
      if(tokens[tok].type == JSMN_STRING){
        if(strncmp((char*)&cmd->payload[tokens[tok].start], LBUS_TOK_DEVICE, strlen(LBUS_TOK_DEVICE)) == 0){
          
          //device changed, reload it
          if(changeDevice(cmd, tok, tokens, MAX_JSON_TOKENS)){
            lielas_log((unsigned char*)"Error handling change device command", LOG_LEVEL_WARN);
            pthread_mutex_unlock(&mutex);
            return -1;
          }
          
        }else if(strncmp((char*)&cmd->payload[tokens[tok].start], LBUS_TOK_NET_TYPE, strlen(LBUS_TOK_NET_TYPE)) == 0){
          
          //change ip
          if(changeNetType(cmd, tok, tokens, MAX_JSON_TOKENS)){
            lielas_log((unsigned char*)"Error handling change net type", LOG_LEVEL_WARN);
            pthread_mutex_unlock(&mutex);
            return -1;
          }
          
        }else if(strncmp((char*)&cmd->payload[tokens[tok].start], LBUS_TOK_RTC, strlen(LBUS_TOK_RTC)) == 0){
          
          //change rtc date/time
          if(changeRtcDateTime(cmd, tok, tokens, MAX_JSON_TOKENS)){
            lielas_log((unsigned char*)"Error handling change rtc date/time", LOG_LEVEL_WARN);
            pthread_mutex_unlock(&mutex);
            return -1;
          }
          
        }
      } //if token == JSMN_STRING
    } // for every token
  }
  pthread_mutex_unlock(&mutex);
  return 0;
}

/********************************************************************************************************************************
 *    static int parsePayload(Lbuscmd *cmd, jsmn_parser *json, jsmntok_t *tokens, int maxtokens)
 ********************************************************************************************************************************/
static int parsePayload(char *payload, jsmn_parser *json, jsmntok_t *tokens, int maxtokens){
  jsmnerr_t r;

  jsmn_init(json);
  r = jsmn_parse(json, payload, tokens, maxtokens);

  if(r != JSMN_SUCCESS){
    return -1;
  }
  return 0;
}

/********************************************************************************************************************************
 *    static int setCmdHandled()
 ********************************************************************************************************************************/
static int setCmdHandled(Lbuscmd *cmd){
  time_t rawtime;
  struct tm *now;

  pthread_mutex_lock(&mutex);
  
  time(&rawtime);
  now = gmtime(&rawtime);

  cmd->handled = 1;
  cmd->tmhandled = *now;

  saveCmd(cmd);
  lbus_remove(cmd);

  pthread_mutex_unlock(&mutex);
  
  return 0;
}


/********************************************************************************************************************************
 *    int getNewId()
 ********************************************************************************************************************************/

static int getNewId(){
	char st[LBUS_SQL_BUFFER_SIZE];
  PGresult *res;
  long id = 0;

  pthread_mutex_lock(&mutex);
  
	if(SQLTableExists("lbus")){
    lielas_log((unsigned char*)"Table lbus does not exist, creating...", LOG_LEVEL_WARN);
	  snprintf(st, LBUS_SQL_BUFFER_SIZE, "CREATE TABLE lielas.lbus( id integer NOT NULL, tmrecv timestamp, usr text, address text, cmd text, payload text, handled boolean, tmhandled timestamp, PRIMARY KEY(id) )");
	  res = SQLexec(st);
	  PQclear(res);
    if(SQLTableExists("lbus")){
      lielas_log((unsigned char*)"Failed to create table lbus", LOG_LEVEL_WARN);
      pthread_mutex_unlock(&mutex);
      return 0;
    }
    lielas_log((unsigned char*)"Table lbus created", LOG_LEVEL_WARN);
    id = 1;
    snprintf(st, LBUS_SQL_BUFFER_SIZE, "INSERT INTO lielas.lbus (id, handled) VALUES ( '%li', 'false')", id);
    res = SQLexec(st);
    PQclear(res);
    pthread_mutex_unlock(&mutex);
    return id;
	}
  snprintf(st, LBUS_SQL_BUFFER_SIZE, "SELECT id FROM lielas.lbus ORDER BY id DESC LIMIT 1");
  res = SQLexec(st);

  id = 1;
  if(PQntuples(res) > 0){
    id = atoi(PQgetvalue(res, 0, 0)) + 1;
  }
  PQclear(res);

  snprintf(st, LBUS_SQL_BUFFER_SIZE, "INSERT INTO lielas.lbus (id, handled) VALUES ( '%li', 'false')", id);
  res = SQLexec(st);
  PQclear(res);

  pthread_mutex_unlock(&mutex);
	return id;
}

/********************************************************************************************************************************
 *    static int saveCmd(Lbuscmd *cmd)
 ********************************************************************************************************************************/

static int saveCmd(Lbuscmd *cmd){
  char st[LBUS_SQL_BUFFER_SIZE];
  PGresult *res;
  char handled[10];
  char timestamp[50];
  char tmhandled[50];

  pthread_mutex_lock(&mutex);
  
  if(cmd->handled){
    strcpy(handled, "true");
  }else{
    strcpy(handled, "false");
  }

  strftime(timestamp, 50, "%Y-%m-%d %H:%M:%S", &cmd->tmrecv);

  if(cmd->handled == 1){
    strftime(tmhandled, 50, "%Y-%m-%d %H:%M:%S", &cmd->tmhandled);
    snprintf(st, LBUS_SQL_BUFFER_SIZE,
           "UPDATE lielas.lbus SET usr='%s', address='%s', cmd='%s', payload='%s', handled='%s', tmrecv='%s', tmhandled='%s' WHERE id ='%li'",
           cmd->user, cmd->address, cmd->cmd, cmd->payload, handled, timestamp, tmhandled, cmd->id);
  }else{
    snprintf(st, LBUS_SQL_BUFFER_SIZE,
           "UPDATE lielas.lbus SET usr='%s', address='%s', cmd='%s', payload='%s', handled='%s', tmrecv='%s' WHERE id ='%li'",
           cmd->user, cmd->address, cmd->cmd, cmd->payload, handled, timestamp, cmd->id);
  }
  res = SQLexec(st);
  PQclear(res);

  pthread_mutex_unlock(&mutex);
  return 0;
}

/********************************************************************************************************************************
 *    int setNextHandle(Lbuscmd *cmd, int offset)
 ********************************************************************************************************************************/
static int setNextHandle(Lbuscmd *cmd, int offset){
  time_t rawtime;
  struct tm *now;
    
  if(cmd->tmnexthandle == NULL){
    cmd->tmnexthandle = malloc(sizeof(struct tm));
    if(cmd->tmnexthandle == NULL){
      return -1;
    }
  }
    
	time(&rawtime);
  now = gmtime(&rawtime);
  *cmd->tmnexthandle = *now;
  cmd->tmnexthandle->tm_sec += offset;
  mktime(cmd->tmnexthandle);
  
  return 0;
}

/********************************************************************************************************************************
 *    int changeDeviceHandler(char *cmd, char *id, int len)
 * 
 * handle lbus command change device
 ********************************************************************************************************************************/
static int changeDevice(Lbuscmd *cmd, int tok, jsmntok_t *tokens, int maxTokens){
    
  char idStr[SQL_STATEMENT_BUF_SIZE];
  int id;
  Ldevice *d;
  char st[SQL_STATEMENT_BUF_SIZE];
  PGresult *res;
  char log[LOG_BUF_LEN];
  int i;
  
  tok += 1;
  if(tok > maxTokens){
    return -1;
  }
  
  strncpy(idStr, (char*)&cmd->payload[tokens[tok].start], tokens[tok].end - tokens[tok].start);
  id = atoi(idStr);
  
  if(id <= 0){
    lielas_log((unsigned char*)"Error parsing lbus command: change device: error parsing device id", LOG_LEVEL_WARN);
    pthread_mutex_unlock(&mutex);
    return -1;
  }  
  
  LDCgetDeviceById(id, &d);
  
  if(d != NULL){
          
    snprintf(log, LOG_BUF_LEN, "reloading device %s", d->mac);
    lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);
            
    for(i = 1; i <= d->moduls; i++){
      snprintf(st, SQL_STATEMENT_BUF_SIZE, "SELECT mint FROM %s.%s WHERE id=%i", LDB_TBL_SCHEMA, LDB_TBL_NAME_MODULS, d->modul[i]->id);
      res = SQLexec(st);
              
      if(PQntuples(res) > 0){
        strcpy(d->modul[i]->mint, PQgetvalue(res, 0, 0));
        snprintf(log, LOG_BUF_LEN, "reloaded %s.%s: new mint is %s", d->mac, d->modul[i]->address, d->modul[i]->mint);
          lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);
      }else{
        snprintf(log, LOG_BUF_LEN, "can't reload modul %s.%s, modul not found", d->mac, d->modul[i]->address);
        lielas_log((unsigned char*)log, LOG_LEVEL_WARN);
      }
              
      PQclear(res);
      snprintf(log, LOG_BUF_LEN, "reloaded modul %s.%s", d->mac, d->modul[i]->address);
      lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);
    }
    setCmdHandled(cmd);
  }else{
    lielas_log((unsigned char*) "can't reload device, device not found", LOG_LEVEL_DEBUG);
    setCmdHandled(cmd);
  } 
  return 0;
}

/********************************************************************************************************************************
 *    int changeNetType(char *id)
 * 
 * handle lbus command change net type
 ********************************************************************************************************************************/
static int changeNetType(Lbuscmd *cmd, int tok, jsmntok_t *tokens, int maxTokens){
  char net_type[LBUS_BUF_SIZE];
  char net_address[LBUS_BUF_SIZE];
  char net_mask[LBUS_BUF_SIZE];
  char net_gw[LBUS_BUF_SIZE];
  char net_dns1[LBUS_BUF_SIZE];
  char net_dns2[LBUS_BUF_SIZE];
  char bashCmd[LBUS_BUF_SIZE];
  char log[LOG_BUF_LEN];
   
  if(cmd->tmnexthandle == 0){
    //first time handling this command, load settings from database and change them
    lielas_log((unsigned char*)"First execution of changeNetType cmd, loading and changing settings", LOG_LEVEL_DEBUG);
    
    lielas_getLDBSetting(net_type, LDB_SQL_SET_NAME_NET_NEW_TYPE, LBUS_BUF_SIZE);
    lielas_getLDBSetting(net_address, LDB_SQL_SET_NAME_NET_NEW_ADR, LBUS_BUF_SIZE);
    lielas_getLDBSetting(net_mask, LDB_SQL_SET_NAME_NET_NEW_MASK, LBUS_BUF_SIZE);
    lielas_getLDBSetting(net_gw, LDB_SQL_SET_NAME_NET_NEW_GATEWAY, LBUS_BUF_SIZE);
    lielas_getLDBSetting(net_dns1, LDB_SQL_SET_NAME_NET_NEW_DNS1, LBUS_BUF_SIZE);
    lielas_getLDBSetting(net_dns2, LDB_SQL_SET_NAME_NET_NEW_DNS2, LBUS_BUF_SIZE);
    
    snprintf(log, LOG_BUF_LEN, "Settings: type %s", net_type);
    lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);
    
    if(!strcmp(net_type, "static")){
      
      snprintf(log, LOG_BUF_LEN, "address %s, netmask %s, gateway %s, dns1 %s, dns2 %s", net_address, net_mask, net_gw, net_dns1, net_dns2);
      lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);
      
      snprintf(bashCmd, LBUS_BUF_SIZE, "sudo /usr/local/lielas/bin/ipchanger set dns %s %s", net_dns1, net_dns2);
      lielas_log((unsigned char*)"Executing:", LOG_LEVEL_DEBUG);
      lielas_log((unsigned char*)bashCmd, LOG_LEVEL_DEBUG);
      if(system(bashCmd)){
        lielas_log((unsigned char*)"Failed to set dns server", LOG_LEVEL_ERROR);
        setCmdHandled(cmd);
        return -1;
      }
      
      snprintf(bashCmd, LBUS_BUF_SIZE, "sudo /usr/local/lielas/bin/ipchanger set %s %s %s %s", net_type, net_address, net_mask, net_gw);
      lielas_log((unsigned char*)"Executing:", LOG_LEVEL_DEBUG);
      lielas_log((unsigned char*)bashCmd, LOG_LEVEL_DEBUG);
      if(system(bashCmd)){
        lielas_log((unsigned char*)"Failed to change network settings", LOG_LEVEL_ERROR);
        setCmdHandled(cmd);
        return -1;
      }
    }else if(!strcmp(net_type, "dhcp")){
      snprintf(bashCmd, LBUS_BUF_SIZE, "sudo /usr/local/lielas/bin/ipchanger set %s", net_type);
      lielas_log((unsigned char*)"Executing:", LOG_LEVEL_DEBUG);
      lielas_log((unsigned char*)bashCmd, LOG_LEVEL_DEBUG);
      if(system(bashCmd)){
        lielas_log((unsigned char*)"Failed to change network settings", LOG_LEVEL_ERROR);
        setCmdHandled(cmd);
        return -1;
      }
    }else{
      lielas_log((unsigned char*)"Failed to change network settings, unknown net_type", LOG_LEVEL_WARN);
      setCmdHandled(cmd);
      return -1;
    }
    
    setNextHandle(cmd, LBUS_NET_RELOGIN_TIME);
    snprintf(log, LOG_BUF_LEN, "network settings changed, reset without relogin at %i:%i:%i", cmd->tmnexthandle->tm_hour, cmd->tmnexthandle->tm_min, cmd->tmnexthandle->tm_sec);
    lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);
    netTypeChanged = 1;
  }else{
    setCmdHandled(cmd);
    
    if(netTypeChanged == 1){
      //no successfull relogin, reset network settings
      
      snprintf(log, LOG_BUF_LEN, "no relogin, resetting network settings");
      lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);
    
      if(system("sudo /usr/local/lielas/bin/ipchanger restore")){
        lielas_log((unsigned char*)"Failed to change network settings", LOG_LEVEL_ERROR);
        return -1;
      }
    }else{
      //save new network settings as standard network settings
      
      snprintf(log, LOG_BUF_LEN, "successfull relogin, saving network settings");
      lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);
      
      if(lielas_getLDBSetting(net_type, LDB_SQL_SET_NAME_NET_NEW_TYPE, LBUS_BUF_SIZE))
        return -1;
      if(lielas_getLDBSetting(net_address, LDB_SQL_SET_NAME_NET_NEW_ADR, LBUS_BUF_SIZE))
        return -1;
      if(lielas_getLDBSetting(net_mask, LDB_SQL_SET_NAME_NET_NEW_MASK, LBUS_BUF_SIZE))
        return -1;
      if(lielas_getLDBSetting(net_gw, LDB_SQL_SET_NAME_NET_NEW_GATEWAY, LBUS_BUF_SIZE))
        return -1;
      if(lielas_getLDBSetting(net_dns1, LDB_SQL_SET_NAME_NET_NEW_DNS1, LBUS_BUF_SIZE))
        return -1;
      if(lielas_getLDBSetting(net_dns2, LDB_SQL_SET_NAME_NET_NEW_DNS2, LBUS_BUF_SIZE))
        return -1;
      
      if(lielas_setLDBSetting(net_type, LDB_SQL_SET_NAME_NET_TYPE))
        return -1;
      if(lielas_setLDBSetting(net_address, LDB_SQL_SET_NAME_NET_ADR))
        return -1;
      if(lielas_setLDBSetting(net_mask, LDB_SQL_SET_NAME_NET_MASK))
        return -1;
      if(lielas_setLDBSetting(net_gw, LDB_SQL_SET_NAME_NET_GATEWAY))
        return -1;
      if(lielas_setLDBSetting(net_dns1, LDB_SQL_SET_NAME_NET_DNS1))
        return -1;
      if(lielas_setLDBSetting(net_dns2, LDB_SQL_SET_NAME_NET_DNS2))
        return -1;
      
    }
  }
  return 0;
}

/********************************************************************************************************************************
 *    int changeRtcDateTime(Lbuscmd *cmd, int tok, jsmntok_t *tokens, int maxTokens){
 * 
 * handle lbus command change rtc date and time
 ********************************************************************************************************************************/
static int changeRtcDateTime(Lbuscmd *cmd, int tok, jsmntok_t *tokens, int maxTokens){
  struct tm dt;
  
  lielas_log((unsigned char*)"Changing RTC date/time", LOG_LEVEL_DEBUG);
  
  tok += 1;
  if(tok > maxTokens){
    lielas_log((unsigned char*)"Failed to change rtc time, error parsing date/time", LOG_LEVEL_WARN);
    return -1;
  }
  
  if(strptime((char*)&cmd->payload[tokens[tok].start], "%Y-%m-%d %H:%M:%S", &dt) == NULL){
    lielas_log((unsigned char*)"Failed to change rtc time, error parsing date/time", LOG_LEVEL_WARN);
    setCmdHandled(cmd);
    return -1;
  }
  
  
  if(rtc_set_dt(&dt)){
    lielas_log((unsigned char*)"Failed to change rtc time, error setting date/time", LOG_LEVEL_WARN);
    setCmdHandled(cmd);
    return -1;
  }
  setCmdHandled(cmd);

  return 0;
}


/********************************************************************************************************************************
 *    void printLbusCmd(Lbuscmd* cmd)
 * 
 * prints cmd to stdout
 ********************************************************************************************************************************/
void printLbusCmd(Lbuscmd* cmd){
#ifdef DEBUG_LBUS
  printf("--------------------------------\n");
  printf("printing command with id %li\n", cmd->id);
  printf("adr. %s usr. %s cmd. %s handled %i\n", cmd-> address, cmd->user, cmd->cmd, cmd->handled);
  printf("payload: %s\n", cmd->payload);
#endif
}

/********************************************************************************************************************************
 *    void printLbusCmdCon()
 * 
 * prints cmd container to stdout
 ********************************************************************************************************************************/
void printLbusCmdCon(){
#ifdef DEBUG_LBUS
  Lbuscmd *cmd;
  
  cmd = lbus_getFirstCmd();
  while(cmd != NULL){
    printLbusCmd(cmd);
    cmd = lbus_getNextCmd();
  }
#endif
}





