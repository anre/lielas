#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lbus.h"
#include "../devicehandler.h"
#include "../log.h"
#include "../sql/sql.h"
#include "../jsmn/jsmn.h"

static Lbuscontainer *lbuscon;
static Lbuscontainer *lbusconPtr;

static int handleDelCmd(Lbuscmd *cmd);
static int handleChgCmd(Lbuscmd *cmd);
static int parsePayload(char *payload, jsmn_parser *json, jsmntok_t *tokens, int maxtokens);
static int getNewId();
static int saveCmd(Lbuscmd *cmd);
static int setCmdHandled(Lbuscmd *cmd);

/********************************************************************************************************************************
 * 		void lbus_init()
 ********************************************************************************************************************************/
int lbus_init(){
  lbuscon = malloc(sizeof(Lbuscontainer));
	if(lbuscon == NULL){
		lielas_log((unsigned char*)"Couldn't allocate memory for lbus container", LOG_LEVEL_ERROR);
		return -1;
	}
  
  lbuscon->llbc = NULL;
  lbuscon->nlbc = NULL;
  lbuscon->lcmd = NULL;
  lbusconPtr = lbuscon;
  
  return 0;
}

/********************************************************************************************************************************
 *    int lbus_add(Lbuscmd *cmd, int saveToDb))
 ********************************************************************************************************************************/

int lbus_add(Lbuscmd *cmd, int saveToDb){
  Lbuscmd *newCmd = malloc(sizeof(Lbuscmd));
  
  if(newCmd == NULL){
		lielas_log((unsigned char*)"Couldn't allocate memory for lbus command", LOG_LEVEL_WARN);
    return -1;
  }
  lbus_getFirstCmd();
  if(lbuscon == NULL){
		lielas_log((unsigned char*)"lbus container not initialized", LOG_LEVEL_ERROR);
    free(newCmd);
    return -1;
  }else if(lbusconPtr->nlbc == NULL&& lbusconPtr->lcmd == NULL){ // first entry
    lbusconPtr->lcmd = newCmd;
    *newCmd = *cmd;

    // save to database
    if(saveToDb){
      if(saveCmd(newCmd)){
        free(newCmd);
        return -1;
      }
    }

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
      return -1;
    }
  }

  return 0;
}

void lbus_remove(Lbuscmd *cmd){
  Lbuscontainer *con;

  lielas_log((unsigned char*)"removing lbus cmd from lbus container", LOG_LEVEL_DEBUG);
  if(cmd == NULL){
    lielas_log((unsigned char*)"can't remove cmd, cmd is null", LOG_LEVEL_WARN);
    return;
  }

  if(lbuscon == NULL){
      lielas_log((unsigned char*)"can't remove cmd, lbus container not initialized", LOG_LEVEL_WARN);
      return;
  }

  con = lbuscon;

  while(con != NULL && con->lcmd->id != cmd->id){
    con = con->nlbc;
  }

  if(con == NULL){
    lielas_log((unsigned char*)"can't remove cmd, cmd not found in container", LOG_LEVEL_WARN);
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
  }else{
    con->llbc->nlbc = con->nlbc;
    con->nlbc->llbc = con->llbc;
    lbus_deleteCmd(con->lcmd);
    free(con);
  }
  lielas_log((unsigned char*)"lbus cmd removed", LOG_LEVEL_DEBUG);

}

/********************************************************************************************************************************
 * 		Lbuscmd *lbus_createCmd()
 ********************************************************************************************************************************/
Lbuscmd *lbus_createCmd(){
	Lbuscmd *cmd = malloc(sizeof(Lbuscmd));
	if(cmd == NULL){
		return NULL;
	}

	//get new id from database

	cmd->id = getNewId();
	if(cmd->id == 0){
		lielas_log((unsigned char*)"Failed to get new lbus cmd id", LOG_LEVEL_WARN);
		free(cmd);
		return NULL;
	}

	cmd->handled = 0;
	cmd->address[0] = 0;
	cmd->cmd[0] = 0;
	cmd->payload[0] = 0;

	return cmd;
}

/********************************************************************************************************************************
 *    void lbus_deleteCmd(Lbuscmd *cmd)
 ********************************************************************************************************************************/
void lbus_deleteCmd(Lbuscmd *cmd){
  free(cmd);
  cmd = 0;
}


/********************************************************************************************************************************
 * 		Lbuscmd *lbus_getFirstCmd()
 ********************************************************************************************************************************/
Lbuscmd *lbus_getFirstCmd(){
  if(lbuscon == NULL)
	  return NULL;
  lbusconPtr = lbuscon;
  return lbusconPtr->lcmd;
}

/********************************************************************************************************************************
 * 		Lbuscmd *lbus_getNextCmd()
 ********************************************************************************************************************************/
Lbuscmd *lbus_getNextCmd(){
  if(lbuscon == NULL)
    return NULL;
  if(lbusconPtr != NULL){
    lbusconPtr = lbusconPtr->nlbc;
  }
  if(lbusconPtr == NULL)
	  return NULL;
  return lbusconPtr->lcmd;
}

/********************************************************************************************************************************
 *    void lbus_handler()
 ********************************************************************************************************************************/
void lbus_handler(){
  Lbuscmd *cmd;

  // look if unhandled cmd are in the buffer
  cmd = lbus_getFirstCmd();
  if(cmd == NULL){
    return;
  }

  //unhandled cmd found
  if(!strcmp(cmd->cmd, LBUS_CMD_DEL )){
    handleDelCmd(cmd);
  }else if(!strcmp(cmd->cmd, LBUS_CMD_CHG )){
    handleChgCmd(cmd);
  }


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
  len += snprintf((char*)&s[len], (maxLen-len), "  \"handled\":\"%s\"\n", (cmd->handled)?"true":"false");
  if(cmd->handled){
    len += strftime((char*)&s[len], (maxLen-len), "  \"tmhandled\":\"%d.%m.%Y %H:%M:%S\",\n", &cmd->tmhandled);
  }else{
    len += snprintf((char*)&s[len], (maxLen-len), "  \"tmhandled\":\"\",\n");
  }
  len += snprintf((char*)&s[len], (maxLen-len), "}\n");

  return len;
}

/********************************************************************************************************************************
 * 		static int handleDelCmd(Lbuscmd *cmd)
 ********************************************************************************************************************************/
static int handleDelCmd(Lbuscmd *cmd){
  jsmntok_t tokens[MAX_JSON_TOKENS];
  jsmn_parser json;
  char errStr[LBUF_MAX_ERR_STR_SIZE];
  int tok;
  int parseError = 0;

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
        if(strncmp((char*)&cmd->payload[tokens[tok].start], LBUS_TOK_TARGET, strlen(LBUS_TOK_TARGET)) == 0){
          tok+=1;
          if(strncmp((char*)&cmd->payload[tokens[tok].start], LBUS_PAYLOAD_DEL_DATA, strlen(LBUS_PAYLOAD_DEL_DATA)) == 0){

          }else if(strncmp((char*)&cmd->payload[tokens[tok].start], LBUS_PAYLOAD_DEL_DB, strlen(LBUS_PAYLOAD_DEL_DB)) == 0){

          }
        }
      } //if token == JSMN_STRING
    } // for every token
  }

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

  //parse address
  if(!strcmp(cmd->address, LBUS_ADDRESS_LIEGW)){
    //address: liegw
    if(parsePayload(cmd->payload, &json, tokens, MAX_JSON_TOKENS) != 0){
      lielas_log((unsigned char*)"JSON parse error parsing lbus payload:", LOG_LEVEL_WARN);
      lbus_printCmd(errStr, LBUF_MAX_ERR_STR_SIZE, cmd);
      lielas_log((unsigned char*)errStr, LOG_LEVEL_WARN);
      return -1;
    }

    for(tok = 0; tok< json.toknext && !parseError; tok++){
      if(tokens[tok].type == JSMN_STRING){
        if(strncmp((char*)&cmd->payload[tokens[tok].start], LBUS_TOK_RUNMODE, strlen(LBUS_TOK_TARGET)) == 0){
          tok+=1;
          if(strncmp((char*)&cmd->payload[tokens[tok].start], LBUS_PAYLOAD_RUNMODE_NORMAL, strlen(LBUS_PAYLOAD_RUNMODE_NORMAL)) == 0){
            lielas_setRunmode(RUNMODE_NORMAL);
            setCmdHandled(cmd);
            return 0;
          }else if(strncmp((char*)&cmd->payload[tokens[tok].start], LBUS_PAYLOAD_RUNMODE_REGISTER, strlen(LBUS_PAYLOAD_RUNMODE_REGISTER)) == 0){
            lielas_setRunmode(RUNMODE_REGISTER);
            setCmdHandled(cmd);
            return 0;
          }
        }
      } //if token == JSMN_STRING
    } // for every token
  }
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

  time(&rawtime);
  now = gmtime(&rawtime);

  cmd->handled = 1;
  cmd->tmhandled = *now;

  saveCmd(cmd);
  lbus_remove(cmd);

  return 0;
}


/********************************************************************************************************************************
 *    int getNewId()
 ********************************************************************************************************************************/

static int getNewId(){
	char st[LBUS_SQL_BUFFER_SIZE];
  PGresult *res;
  long id = 0;

	if(SQLTableExists("lbus")){
    lielas_log((unsigned char*)"Table lbus does not exist, creating...", LOG_LEVEL_WARN);
	  snprintf(st, LBUS_SQL_BUFFER_SIZE, "CREATE TABLE lielas.lbus( id integer NOT NULL, tmrecv timestamp, usr text, address text, cmd text, payload text, handled boolean, tmhandled timestamp, PRIMARY KEY(id) )");
	  res = SQLexec(st);
	  PQclear(res);
    if(SQLTableExists("lbus")){
      lielas_log((unsigned char*)"Failed to create table lbus", LOG_LEVEL_WARN);
      return 0;
    }
    lielas_log((unsigned char*)"Table lbus created", LOG_LEVEL_WARN);
    id = 1;
    snprintf(st, LBUS_SQL_BUFFER_SIZE, "INSERT INTO lielas.lbus (id, handled) VALUES ( '%li', 'false')", id);
    res = SQLexec(st);
    PQclear(res);
    return id;
	}
  snprintf(st, LBUS_SQL_BUFFER_SIZE, "SELECT id FROM lielas.lbus WHERE handled='false' ORDER BY id DESC LIMIT 1");
  res = SQLexec(st);

  if(PQntuples(res) > 0){
    id = atoi(PQgetvalue(res, 0, 0)) + 1;
  }
  PQclear(res);

  snprintf(st, LBUS_SQL_BUFFER_SIZE, "INSERT INTO lielas.lbus (id, handled) VALUES ( '%li', 'false')", id);
  res = SQLexec(st);
  PQclear(res);

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

  if(cmd->handled){
    strcpy(handled, "true");
  }else{
    strcpy(handled, "false");
  }

  strftime(timestamp, 50, "%Y-%m-%d %H:%M:%S", &cmd->tmrecv);

  if(cmd->handled == 1){
    strftime(tmhandled, 50, "%Y-%m-%d %H:%M:%S", &cmd->tmhandled);
    snprintf(st, LBUS_SQL_BUFFER_SIZE,
           "UPDATE lielas.lbus SET usr='%s', address='%s', cmd='%s', payload='%s', handled='%s', tmrecv='%s' tmhandled='%s' WHERE id ='%li'",
           cmd->user, cmd->address, cmd->cmd, cmd->payload, handled, timestamp, tmhandled, cmd->id);
  }else{
    snprintf(st, LBUS_SQL_BUFFER_SIZE,
           "UPDATE lielas.lbus SET usr='%s', address='%s', cmd='%s', payload='%s', handled='%s', tmrecv='%s' WHERE id ='%li'",
           cmd->user, cmd->address, cmd->cmd, cmd->payload, handled, timestamp, cmd->id);
  }
  res = SQLexec(st);
  PQclear(res);

  return 0;
}



