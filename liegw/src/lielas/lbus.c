#include <stdio.h>
#include <stdlib.h>

#include "lbus.h"
#include "../log.h"

static Lbuscontainer *lbuscon;
static Lbuscontainer *lbusconPtr;

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

int lbus_add(Lbuscmd *cmd){
  Lbuscmd *newCmd = malloc(sizeof(Lbuscmd));
  
  if(newCmd == NULL){
		lielas_log((unsigned char*)"Couldn't allocate memory for lbus command", LOG_LEVEL_WARN);
    return -1;
  }
  
  if(lbuscon == NULL){
		lielas_log((unsigned char*)"lbus container not initialized", LOG_LEVEL_ERROR);
    free(newCmd);
    return -1;
  }else if(lbusconPtr->nlbc == NULL&& lbusconPtr->lcmd == NULL){ // first entry
    lbusconPtr->lcmd = newCmd;
    *newCmd = *cmd;
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
  return 0;
}

int lbus_remove(Lbuscmd *cmd);

/********************************************************************************************************************************
 * 		Lbuscmd *lbus_getFirstCmd()
 ********************************************************************************************************************************/
Lbuscmd *lbus_getFirstCmd(){
  lbusconPtr = lbuscon;
  return lbusconPtr->lcmd;
}

/********************************************************************************************************************************
 * 		Lbuscmd *lbus_getNextCmd()
 ********************************************************************************************************************************/
Lbuscmd *lbus_getNextCmd(){
  if(lbusconPtr != NULL){
    lbusconPtr = lbusconPtr->nlbc;
  }
  return lbusconPtr->lcmd;
}


