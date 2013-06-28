#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/timex.h>
#include <errno.h>

#include "rtc.h"
#include "rtc4162.h"
#include "../log.h"


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
    exit(0);
    
    //ntp not synchronized, wait for time or sync
    
    
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
 
 
