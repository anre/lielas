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

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/timex.h>
#include <errno.h>

#include "rtc.h"
#include "rtc4162.h"
#include "../log.h"


const char rtc_state_text_uninitialized[] = { "uninitialized" };
const char rtc_state_text_ok[]            = { "ok" };
const char rtc_state_text_not_synced[]    = { "not synced" };

static int rtc_state;

int testdt(struct tm *dt);

/********************************************************************************************************************************
 * 		int rtc_init()
 ********************************************************************************************************************************/
int rtc_init(){
  struct tm dt;
  struct ntptimeval ntv;
	time_t rawtime;
	struct tm *now;
  int timenotvalid = 0;
  
  char cmd[100];
  
  rtc_state = RTC_STATE_UNINITIALIZED;
  
  //init rtc modul
  if(rtc4162_init()){
    return RTC_ERROR;
  }
  
  //read time
  timenotvalid = rtc4162_get(&dt);
  
  if(!timenotvalid){
    timenotvalid = testdt(&dt);
  }
  
  if(timenotvalid){
    // time not valid, check if ntp is synchronized
    lielas_log((unsigned char*)"rtc: rtc-module time not valid, check if system time is synced", LOG_LEVEL_DEBUG);
    
    if(ntp_gettime(&ntv)){
      lielas_log((unsigned char*)"rtc: error executing ntp_gettime()", LOG_LEVEL_WARN);
      return -1;
    }
    
    if(errno == TIME_ERROR){
      //ntp not synchorinzed
      lielas_log((unsigned char*)"rtc: rtc-module time not valid and ntp not synced", LOG_LEVEL_WARN);
      rtc_state = RTC_STATE_NOT_SYNCED;
      return -1;
      
    }else{
      //ntp synchronized, set rtc time
      lielas_log((unsigned char*)"rtc: rtc-module time not valid but ntp synced, setting rtc-module time", LOG_LEVEL_DEBUG);
      time(&rawtime);
      now = gmtime(&rawtime);
      
      rtc4162_set(now);
      
    }
    return RTC_TIME_NOT_VALID;
  }
  
  lielas_log((unsigned char*)"rtc: rtc time valid, setting system time", LOG_LEVEL_DEBUG);
  
  //set system time
  snprintf(cmd, 100, "sudo date --set %.4i-%.2i-%.2i", (dt.tm_year+1900), (dt.tm_mon+1), dt.tm_mday);
  if(system(cmd)){
    return RTC_ERROR;
  }
  
  snprintf(cmd, 100, "sudo date --set %.2i:%.2i:%.2i", dt.tm_hour, dt.tm_min, dt.tm_sec);
  if(system(cmd)){
    return RTC_ERROR;
  }
  
  lielas_log((unsigned char*)"rtc: system time set", LOG_LEVEL_DEBUG);
  
  rtc_state = RTC_STATE_OK;  
  
  return 0;
}

/********************************************************************************************************************************
 * 		int rtc_get_state()
 ********************************************************************************************************************************/
int rtc_get_state(){
  return rtc_state;
}


/********************************************************************************************************************************
 * 		const char *rtc_get_state_text()
 ********************************************************************************************************************************/
const char *rtc_get_state_text(){
  switch(rtc_state){
    case RTC_STATE_UNINITIALIZED:
      return rtc_state_text_uninitialized;
    case RTC_STATE_OK:
      return rtc_state_text_ok;
    case RTC_STATE_NOT_SYNCED:
      return rtc_state_text_not_synced;
  }
  return NULL;
}


/********************************************************************************************************************************
 * 		int testdt(struct tm *dt)
 ********************************************************************************************************************************/
int testdt(struct tm *dt){
	if(dt->tm_hour < 0 || dt->tm_hour > 23)
		return -1;
	if(dt->tm_min < 0 || dt->tm_min > 59)
		return -1;
	if(dt->tm_sec < 0 || dt->tm_sec > 59)
		return -1;
	if(dt->tm_year < 100 || dt->tm_year > 200)
		return -1;
	if(dt->tm_mon < 0 || dt->tm_mon > 11)
		return -1;
	if(dt->tm_mday < 1 || dt->tm_mday > 31)
		return -1;

	return 0;  
}
 
 
