/*
*
*	Copyright (c) 2013 Andreas Reder
*	Author      : Andreas Reder <andreas@reder.eu>
*	File		: 
*
*	This File is part of lieweb, a web-gui for osd-compatible devices
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

#include "lielas/devicecontainer.h"
#include "coap/coapserver.h"
#include "log.h"
#include "devicehandler.h"
#include "signals.h"

int savePID();
void deletePID();

/*
 * 		main
 */
int main(void) {

	//pid = fork();

	//setbuf(stdout, NULL);
	printf("starting liewebgw\n");
	fflush(stdout);

	printf("saving pid\n");
	if(savePID() != 0){
		lielas_log((unsigned char*)"Error saving PID", LOG_LEVEL_ERROR);
		return -1;
	}

	printf("init signal handlers\n");
	fflush(stdout);
	if(sig_init() != 0){
		lielas_log((unsigned char*)"Error initializing signal handlers", LOG_LEVEL_ERROR);
		return -1;
	}

	printf("init ldc\n");
	fflush(stdout);
	if(LDCinit() != 0){
		lielas_log((unsigned char*)"Error initializing device container", LOG_LEVEL_ERROR);
		return -1;
	}
	printf("load debices\n");
	fflush(stdout);
	if(LDCloadDevices() != 0){
		lielas_log((unsigned char*)"Error loading devices", LOG_LEVEL_ERROR);
		return -2;
	}
	printf("init coap\n");
	fflush(stdout);
	if(COAPinit() != 0){
		lielas_log((unsigned char*)"Error initializing COAP Server", LOG_LEVEL_ERROR);
		return -3;
	}


	printf("Lielasd COAP server successfully started\n");
	while(!sig_sigint_received()){
		//COAPhandleServer();
		//HandleDevices();
		//LDCcheckForNewDevices();
		if(sig_sigusr1_received()){
			sig_reset_sigusr1();
			lielas_log((unsigned char*)"received SIGUSR1", LOG_LEVEL_DEBUG);
			fflush(stdout);
		}
	}
	printf("Shutting down server\n");
	return EXIT_SUCCESS;
}

int savePID(){
	FILE *f = fopen("pid", "a");
	if(f == NULL){
		return -1;
	}
	fprintf(f, "%lu\n", (unsigned long) getpid());
	fclose(f);
	atexit(deletePID);
	return 0;
}

void deletePID(){
	unlink("pid");
}
