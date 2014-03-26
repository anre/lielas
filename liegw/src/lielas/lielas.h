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


#ifndef LIELAS_H_
#define LIELAS_H_

#define IPV6_STR_LEN                      50
#define CHANNEL_ATTR_STR_LEN              50
#define INT_STR_LEN                       50
#define MAC_STR_LEN                       50

#define SCAN_NEW_DEVICES_INTERVAL         300
#define MAX_SCAN_NEW_DEVICES_INTERVAL     3600
#define GET_NEXT_DEVICE_DATA_INTERVAL     90
#define GET_FIRST_DEVICE_DATA_INTERVAL    90
#define MAX_GET_DEVICE_DATA_INTERVAL      3600

#define MAX_TIME_DIFF                     4

#define LIELAS_STD_MINT                   300
#define LIELAS_STD_PINT                   2
#define LIELAS_STD_AINT                   300

#define MAX_MODULS                        20
#define MAX_CHANNELS                      20

#define LIELAS_SUPPLY_STATE_OK            "ok"
#define LIELAS_SUPPLY_STATE_LOW           "low"
#define LIELAS_SUPPLY_LOW_VOLTAGE         3.0

#endif /* LIELAS_H_ */
