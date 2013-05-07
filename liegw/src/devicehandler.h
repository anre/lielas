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

#ifndef DEVICEHANDLER_C
#define DEVICEHANDLER_C

#include "lielas/lielas.h"
#include "lielas/device.h"

void HandleDevices();


#define CMDBUFFER_SIZE			500
#define DATABUFFER_SIZE			20000
#define VALUEBUFFER_SIZE		20
#define MAX_VALUES_IN_PAKET		1000

#define MAX_DATA_SEND_TIME		40

#define GET_FIRST_VALUE_DATE	"2000.01.01 00:00:00"

#define VALUE_DECIMAL_POINT		','
#define VALUE_SEPARATOR			';'

typedef struct datapaket_struct{
	char value[VALUEBUFFER_SIZE];
	Ldevice *d;
	Lmodul *m;
	Lchannel* c;
	struct tm *dt;	// datetime
} datapaket;

typedef struct datapaketcontainer_struct{
	Ldevice* d;
	Lmodul* m;
	int datapakets;		// number of datapakets
	datapaket *dp[MAX_VALUES_IN_PAKET];
	int dec;	// number of digits after decimal point
} datapaketcontainer;

#define RUNMODE_NORMAL          0
#define RUNMODE_REGISTER        1

int lielas_getRunmode();
int lielas_setRunmode(int mode);
void lielas_runmodeHandler();
struct tm *lielas_getEndRegModeTimer();

#endif

