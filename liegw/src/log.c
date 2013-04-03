#include "log.h"
#include <time.h>
#include <stdio.h>

#define LOG_LEVEL	LOG_LEVEL_DEBUG
#define LOG_TO		LOG_TO_STDOUT

void lielas_log(unsigned char *msg, int level){
	char timestamp[50];
	time_t rawtime;
	struct tm *now;
	
	if(level < LOG_LEVEL)	//log msg not relevant for current log level
		return;

	//create timestamp
	time(&rawtime);
	now = gmtime(&rawtime);
	strftime(timestamp, 50, "[%d.%m.%Y %H:%M:%S]", now);
	
	#if LOG_TO == LOG_TO_STDOUT
		printf("%s%s\n", timestamp, msg);
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
