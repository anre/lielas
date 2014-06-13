/*
*
*	Copyright (c) 2014 Andreas Reder
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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "resetButton.h"
#include "lielas/lbus.h"
#include "lielas/ldb.h"

static int getValue();

int rb_init(){
  int status;
  int bttnState;
  char cmd[1000];

  // check if directory already exists
  if(access(RB_DIR_PATH, F_OK)){
    snprintf(cmd, sizeof(cmd), "echo 9 > %s", RB_SYS_PATH);
    status = system(cmd);
    
    if(status){
      return -1;
    }
    sleep(1);
    
    if(access(RB_DIR_PATH, F_OK)){
      return -2;
    }
  }
  
  //get value
  bttnState = getValue();
  if(!(bttnState == RB_BTTN_STATE_RELEASED || bttnState == RB_BTTN_STATE_PRESSED)){
    return -3;
  }

  return 0;
}


void* rb_handler(){
  int counter = 0;
  Lbuscmd *cmd;
  time_t rawtime;
  struct tm *tmnow;

  while(1){
    if(getValue() == RB_BTTN_STATE_PRESSED){
      counter += 1;
      if(counter == 4){
        cmd = lbus_createCmd(0);
        if(cmd != NULL){
          //change settings in database
          lielas_setLDBSetting("dhcp", LDB_SQL_SET_NAME_NET_NEW_TYPE);
          lielas_setLDBSetting("", LDB_SQL_SET_NAME_NET_NEW_ADR);
          lielas_setLDBSetting("", LDB_SQL_SET_NAME_NET_NEW_MASK);
          lielas_setLDBSetting("", LDB_SQL_SET_NAME_NET_NEW_GATEWAY);
          lielas_setLDBSetting("", LDB_SQL_SET_NAME_NET_NEW_DNS1);
          lielas_setLDBSetting("", LDB_SQL_SET_NAME_NET_NEW_DNS2);
          
          //send lbus command
          snprintf(cmd->address, LBUF_MAX_ADDRESS_SIZE, "liegw");
          snprintf(cmd->user, LBUF_MAX_ADDRESS_SIZE, "0");
          snprintf(cmd->cmd, LBUF_MAX_ADDRESS_SIZE, "chg");
          snprintf(cmd->payload, LBUF_MAX_ADDRESS_SIZE, "{\n  \"net_type\": \"DHCP\"\n}");
          time(&rawtime);
          tmnow = gmtime(&rawtime);
          cmd->tmrecv = *tmnow;
          lbus_add(cmd, 1);
        }
      }
    }else{
      counter = 0;
    }
    sleep(1);  
  }
  return NULL;
}

int getValue(){
  FILE *f;
  int val;
  
  f = fopen(RB_VAL_PATH, "r");
  if(f == NULL){
    return -1;
  }
  
  val = fgetc(f);
  
  fclose(f);
return val;  
}
