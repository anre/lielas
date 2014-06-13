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

#include "powerButton.h"
#include "lielas/lbus.h"
#include "lielas/ldb.h"

static int getValue();

int pb_init(){
  int status;
  int bttnState;
  char cmd[1000];

  // check if directory already exists
  if(access(PB_DIR_PATH, F_OK)){
    snprintf(cmd, sizeof(cmd), "echo 8 > %s", PB_SYS_PATH);
    status = system(cmd);
    
    if(status){
      return -1;
    }
    sleep(1);
    
    if(access(PB_DIR_PATH, F_OK)){
      return -2;
    }
  }
  
  //get value
  bttnState = getValue();
  if(!(bttnState == PB_BTTN_STATE_RELEASED || bttnState == PB_BTTN_STATE_PRESSED)){
    return -3;
  }

  return 0;
}


void* pb_handler(){
  int counter = 0;

  while(1){
    if(getValue() == PB_BTTN_STATE_PRESSED){
      counter += 1;
      if(counter == 2){
        system("sudo shutdown -h now");
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
  
  f = fopen(PB_VAL_PATH, "r");
  if(f == NULL){
    return -1;
  }
  
  val = fgetc(f);
  
  fclose(f);
return val;  
}
