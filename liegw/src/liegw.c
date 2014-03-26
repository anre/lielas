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
#include <curl/curl.h>

#include "lielas/devicecontainer.h"
#include "tcpserver.h"
#include "log.h"
#include "devicehandler.h"
#include "lielas/lbus.h"
#include "lielas/ldb.h"
#include "settings.h"

#include "rtc/rtc.h"

#include "coap/libcoap/coap.h"
#include "coap/mycoap.h"

#include "resetButton.h"
#include "powerButton.h"

#include "lielas/lwp.h"
/*
 * 		main
 */
 #define VERSION_H  1
 #define VERSION_M  0
 #define VERSION_L  0
 
int main(void) {
  pthread_t tcpserverThread;
  pthread_t rbHandlerThread;
  pthread_t pbHandlerThread;
  time_t rawtime;
  struct tm *now;
  char log[LOG_BUF_LEN];
  int error;
  
  //pid = fork();
  
  //setbuf(stdout, NULL);
  snprintf(log, LOG_BUF_LEN, "starting liegw %i.%i.%i", VERSION_H, VERSION_M, VERSION_L);
  lielas_log((unsigned char*)log, LOG_LEVEL_DEBUG);

  lielas_log((unsigned char*)"setting timezone", LOG_LEVEL_DEBUG);
  putenv("TZ=CUT0");
  tzset();
  
  lielas_log((unsigned char*)"load settings", LOG_LEVEL_DEBUG);
  if(set_load()){
      lielas_log((unsigned char*)"Error loading settings from config.json", LOG_LEVEL_ERROR);
      return -1;
  }
  
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

  lielas_log((unsigned char*)"Starting coap server thread", LOG_LEVEL_DEBUG);
  pthread_create(&tcpserverThread, NULL, tcpserver, NULL);
  
  lielas_log((unsigned char*)"init reset Button", LOG_LEVEL_DEBUG);
  error = rb_init();
	if(error != 0){
    snprintf(log, LOG_BUF_LEN, "Error initializing reset button with error code: %d", error);
		lielas_log((unsigned char*)log, LOG_LEVEL_ERROR);
		return -5;
	} 
  lielas_log((unsigned char*)"Starting reset button handler thread", LOG_LEVEL_DEBUG);
  pthread_create(&rbHandlerThread, NULL, rb_handler, NULL);
  
  lielas_log((unsigned char*)"init power Button", LOG_LEVEL_DEBUG);
  error = pb_init();
	if(error != 0){
    snprintf(log, LOG_BUF_LEN, "Error initializing power button with error code: %d", error);
		lielas_log((unsigned char*)log, LOG_LEVEL_ERROR);
		return -5;
	} 
  lielas_log((unsigned char*)"Starting power button handler thread", LOG_LEVEL_DEBUG);
  pthread_create(&pbHandlerThread, NULL, pb_handler, NULL);  

	lielas_log((unsigned char*)"Lielasd COAP server successfully started", LOG_LEVEL_DEBUG);
  
  while(rtc_get_state() == RTC_STATE_NOT_SYNCED){
      sleep(1);
      time(&rawtime);
      now = gmtime(&rawtime);
      if(now->tm_sec <= 1){
        rtc_init();
      }
      lbus_handler();
  }
  
	lielas_log((unsigned char*)"Lielasd successfully started", LOG_LEVEL_DEBUG);
  
	while(1){
		lbus_handler();
		LDCcheckForNewDevices();
		HandleDevices();
	}
	lielas_log((unsigned char*)"Shutting down server", LOG_LEVEL_DEBUG);
	return EXIT_SUCCESS;
}

