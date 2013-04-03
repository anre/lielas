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

#ifndef SETTINGS_H_
#define SETTINGS_H_

#define SET_MAX_LINE_LEN	256
#define SET_ATTR_LEN		256


#define SET_CMD_SQL_USER	"sqluser="
#define SET_CMD_SQL_PASS	"sqlpassword="
#define SET_CMD_SQL_DB		"sqldb="
#define SET_CMD_SQL_HOST	"sqlhost="
#define SET_CMD_SQL_PORT	"sqlport="
#define SET_CMD_COAP_ADDR	"coapaddress="
#define SET_CMD_COAP_PORT	"coapport="
#define SET_CMD_GW_ADDR		"gwaddress="
#define SET_CMD_GW_PORT		"gwport="

#define SET_STD_SQL_USER	"postgres"
#define SET_STD_SQL_PASS	"postgres"
#define SET_STD_SQL_DB		"lielas"
#define SET_STD_SQL_HOST	"192.168.0.100"
#define SET_STD_SQL_PORT	"5432"
#define SET_STD_COAP_ADDR	"::"
#define SET_STD_COAP_PORT	"5683"
#define SET_STD_GW_ADDR		"2001:15c0:666d:1::ff:fe00:1"
#define SET_STD_GW_PORT		"80"

const char *set_getSqlUser();
const char *set_getSqlPass();
const char *set_getSqlDb();
const char *set_getSqlHost();
const char *set_getSqlPort();

const char *set_getCoapAddr();
const char *set_getCoapPort();

const char *set_getGatewaynodeAddr();
const char *set_getGatewaynodePort();

int set_load();
#endif /* SETTINGS_H_ */
