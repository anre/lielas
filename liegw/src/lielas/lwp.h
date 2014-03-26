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

#ifndef LWP_H
#define LWP_H

#include "lielas.h"
#include <stdint.h>
#include <time.h>

#define LWP_RESOURCE_WKC                ".well-known/core"
#define LWP_RESOURCE_CHANNEL            "channel"
#define LWP_RESOURCE_DATABASE           "database"
#define LWP_RESOURCE_DEVICE             "device"
#define LWP_RESOURCE_INFO               "info"
#define LWP_RESOURCE_LOGGER             "logger"
#define LWP_RESOURCE_MODUL              "modul"
#define LWP_RESOURCE_NETWORK            "network"

#define LWP_ATTR_INFO_NAME              "name"
#define LWP_ATTR_INFO_SW_VER            "sw_version"
#define LWP_ATTR_DEVICE_SUPPLY          "supply"
#define LWP_ATTR_DEVICE_MODULE_CNT      "module_cnt"
#define LWP_ATTR_DEVICE_DATETIME        "datetime"
#define LWP_ATTR_MODUL_MUX              "mux"
#define LWP_ATTR_MODUL_CHANNEL_CNT      "channel_cnt"
#define LWP_ATTR_MODUL_PROTOCOL         "protocol"
#define LWP_ATTR_NETWORK_PANID          "panid"
#define LWP_ATTR_NETWORK_CHANNEL        "channel"
#define LWP_ATTR_NETWORK_KEY            "key"
#define LWP_ATTR_NETWORK_CYCLING_MODE   "cycling_mode"
#define LWP_ATTR_NETWORK_CYCLING_TIME   "cycling_time"
#define LWP_ATTR_NETWORK_MAC            "mac"
#define LWP_ATTR_DATABASE_DATETIME      "datetime"
#define LWP_ATTR_CHANNEL_VALUE          "value"
#define LWP_ATTR_CHANNEL_UNIT           "unit"
#define LWP_ATTR_CHANNEL_TYPE           "type"
#define LWP_ATTR_CHANNEL_CLASS          "class"
#define LWP_ATTR_CHANNEL_EXPONENT       "exponent"
#define LWP_ATTR_LOGGER_STATE           "state"
#define LWP_ATTR_LOGGER_INTERVAL        "interval"
#define LWP_ATTR_LOGGER_MIN             "min"
#define LWP_ATTR_LOGGER_MAX             "max"
#define LWP_ATTR_LOGGER_STEP            "step"


#define LWP_MAX_ATTRIBUTES      10
#define LWP_MAX_ATTR_NAME_LEN   100
#define LWP_MAX_RES_NAME_LEN    100
#define LWP_MAX_WKC_LEN         2000

#define LWP_COMP_DT_LEN         4

#define LWP_CENTURY_OFFSET      100

#define LWP_CYCLE_MODE_OFF      0
#define LWP_CYCLE_MODE_ON       1

typedef struct lwp_attr_struct{
  char name[LWP_MAX_ATTR_NAME_LEN];
} lwp_attr;

typedef struct lwp_resource_struct{
  char name[LWP_MAX_RES_NAME_LEN];
  int attributes;
  lwp_attr attr[LWP_MAX_ATTRIBUTES];
} lwp_resource;

typedef struct lwp_wkc_struct{
  lwp_resource info;
  lwp_resource device;
  lwp_resource modul;
  lwp_resource network;
  lwp_resource database;
  lwp_resource logger;
  lwp_resource channel[MAX_CHANNELS];
  int channels;
} lwp_wkc;



void lwp_init_wkc(lwp_wkc *wkc);
void lwp_init_resource(lwp_resource *res);
void lwp_init_attr(lwp_attr *attr);
int lwp_parse_wkc(char *str, lwp_wkc *wkc);
lwp_resource *lwp_get_res_by_name( char* str, lwp_wkc *wkc);
int lwp_add_attr(char* name, lwp_resource *res);
void lwp_print_wkc(lwp_wkc *wkc, char* str);
int lwp_get_attr_value(char *attr, int attrSize, char *val, int size, int pos);
void lwp_compdt_to_struct_tm(uint8_t *cdt, struct tm *dt);
void lwp_mac_to_std_mac(char* dest, char* src);
int lwp_get_vbat(char* attr, int len);
#endif


