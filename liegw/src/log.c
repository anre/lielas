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
		FILE *f = fopen("/usr/local/lielas/logs/liegw.log", "a");
		if(f != NULL){
			fprintf(f, "%s%s%s\n", timestamp, lvlStr, msg);
			fclose(f);
		}else{
			printf("error: failed to open logfile\n");
		}
	#elif LOG_TO == LOG_TO_SYSLOG
	
	#endif	
}
