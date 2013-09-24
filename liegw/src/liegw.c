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
#include <signal.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <pthread.h>
#include <math.h>

#include "lielas/devicecontainer.h"
#include "coap/coapserver.h"
#include "log.h"
#include "devicehandler.h"
#include "lielas/lbus.h"
#include "lielas/ldb.h"

#include "rtc/rtc.h"

/*
 * 		main
 */
int main(void) {
  pthread_t coapserverThread;

  //pid = fork();
  
  //system("sudo /usr/local/lielas/bin/ipchanger set dhcp");
  //exit(0);
  
  //setbuf(stdout, NULL);
  lielas_log((unsigned char*)"starting liewebgw", LOG_LEVEL_DEBUG);

  lielas_log((unsigned char*)"setting timezone", LOG_LEVEL_DEBUG);
  putenv("TZ=CUT0");
  tzset();
  
  lielas_log((unsigned char*)"init rtc", LOG_LEVEL_DEBUG);
  if(rtc_init()){
    if(rtc_get_state() == RTC_STATE_NOT_SYNCED){
      lielas_log((unsigned char*)"RTC state not synced, starting in reduced mode", LOG_LEVEL_WARN);
    }else{
      lielas_log((unsigned char*)"Error initializing rtc", LOG_LEVEL_ERROR);
      return -1;
    }
  }
  
  lielas_log((unsigned char*)"init database", LOG_LEVEL_DEBUG);
  if(lielas_createTables() != 0){
    lielas_log((unsigned char*)"Error initializing database", LOG_LEVEL_ERROR);
    return -1;
  }

	lielas_log((unsigned char*)"init ldc", LOG_LEVEL_DEBUG);
	if(LDCinit() != 0){
		lielas_log((unsigned char*)"Error initializing device container", LOG_LEVEL_ERROR);
		return -1;
	}
	
	lielas_log((unsigned char*)"loading devices", LOG_LEVEL_DEBUG);
	LDCloadDevices();
  
  lielas_log((unsigned char*)"init lbus", LOG_LEVEL_DEBUG);
	if(lbus_init() != 0){
		lielas_log((unsigned char*)"Error initializing lbus", LOG_LEVEL_ERROR);
		return -2;
	}
 
  lielas_log((unsigned char*)"load lbus commands", LOG_LEVEL_DEBUG);
	if(lbus_load() != 0){
		lielas_log((unsigned char*)"Error loading lbus commands", LOG_LEVEL_ERROR);
		return -4;
	} 
	
	lielas_log((unsigned char*)"init coap", LOG_LEVEL_DEBUG);
	if(COAPinit() != 0){
		lielas_log((unsigned char*)"Error initializing COAP Server", LOG_LEVEL_ERROR);
		return -5;
	}

  lielas_log((unsigned char*)"set runmode to normal mode", LOG_LEVEL_DEBUG);
	if(lielas_setRunmode(RUNMODE_NORMAL) != 0){
    lielas_log((unsigned char*)"Error setting runmode to normal mode", LOG_LEVEL_ERROR);
    return -6;
	}

  lielas_log((unsigned char*)"Starting coap server thread", LOG_LEVEL_DEBUG);
  pthread_create(&coapserverThread, NULL, COAPhandleServer, NULL);

	lielas_log((unsigned char*)"Lielasd COAP server successfully started", LOG_LEVEL_DEBUG);
  
  while(rtc_get_state() == RTC_STATE_NOT_SYNCED){
      sleep(1);
      lbus_handler();
  }
  
	lielas_log((unsigned char*)"Lielasd successfully started", LOG_LEVEL_DEBUG);
  
	while(1){
		lbus_handler();
		if(lielas_getRunmode() == RUNMODE_NORMAL){
			LDCcheckForNewDevices();
			HandleDevices();
		}else if(lielas_getRunmode() == RUNMODE_REGISTER){
			lielas_runmodeHandler();
		}
	}
	lielas_log((unsigned char*)"Shutting down server", LOG_LEVEL_DEBUG);
	return EXIT_SUCCESS;
}

