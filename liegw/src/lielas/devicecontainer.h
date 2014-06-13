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

#ifndef DEVICECONTAINER_H
#define DEVICECONTAINER_H

#include "device.h"


#define CLIENT_BUFFER_LEN	1000
#define IPV6_ADR_BUF_LEN	50
#define RPL_TABLE_LEN     20000

#define ID_TYPE_DEVICE	0
#define ID_TYPE_MODUL	1
#define ID_TYPE_CHANNEL	2

#define LDC_DEVICE_NOT_FOUND			-2
#define LDC_FAILED_TO_ALOCATE_MEMORY	-3

#if 1
  #define DC_USE_RPL_WEB_SERVER
#else
  #define DC_USE_RPL_COAP_SERVER
#endif


struct Ldc_struct{
	Ldevice *d;
	struct Ldc_struct *ndc;
	struct Ldc_struct *ldc;
}Ldc;

int LDCinit();
int LDCadd(Ldevice *d);
int LDCremove(Ldevice *d);
int LDCgetNumberOfDevices();
int LDCloadDevices();
int LDCsaveNrOfDatapakte(Ldevice *d);
int LDCsaveUpdatedDevice(Ldevice *d);
int LDCgetDeviceByAddress(const char* adr, Ldevice **d);
int LDCgetDeviceById(int id, Ldevice **d);
void LDCcheckForNewDevices();
Ldevice *LDCgetFirstDevice();
Ldevice *LDCgetNextDevice();
void LDCdelete();


#endif /* DEVICECONTAINER_H */

