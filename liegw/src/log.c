#include "log.h"
#include <time.h>
#include <stdio.h>
#include <string.h>

#define LOG_LEVEL	LOG_LEVEL_DEBUG
#define LOG_TO		LOG_TO_STDOUT

void lielas_log(unsigned char *msg, int level){
	char timestamp[50];
	time_t rawtime;
	struct tm *now;
	char lvlStr[50];
	
	if( LOG_LEVEL < level)	//log msg not relevant for current log level
		return;

	if(level == LOG_LEVEL_DEBUG){
	  strcpy(lvlStr, "[DEBUG]");
	}else if(level == LOG_LEVEL_WARN){
    strcpy(lvlStr, "[WARNING]");
  }else if(level == LOG_LEVEL_ERROR){
    strcpy(lvlStr, "[ERROR]");
  }

	//create timestamp
	time(&rawtime);
	now = gmtime(&rawtime);
	strftime(timestamp, 50, "[%d.%m.%Y %H:%M:%S]", now);
	
	#if LOG_TO == LOG_TO_STDOUT
		printf("%s%s%s\n", timestamp, lvlStr, msg);
		fflush(stdout);
	#elif LOG_TO == LOG_TO_FILE
		FILE *f = fopen("lielas.log", "a");
		if(f != NULL){
			fprintf("%s%s\n", timestamp, msg);
			fclose(f);
		}else{
			printf("error: failed to open logfile\n");
		}
	#elif LOG_TO == LOG_TO_SYSLOG
	
	#endif	
}
