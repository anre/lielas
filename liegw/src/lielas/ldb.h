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

#ifndef LDB_H
#define LDB_H

#define LDB_SQL_BUFFER_SIZE  1000

#define LDB_TBL_SCHEMA  "lielas"

#define LDB_TBL_NAME_LBUS           "lbus"
#define LDB_TBL_NAME_USERS          "users"
#define LDB_TBL_NAME_DEVICE_GROUPS  "device_groups"
#define LDB_TBL_NAME_USER_GROUPS    "user_groups"
#define LDB_TBL_NAME_DEVICES        "devices"
#define LDB_TBL_NAME_MODULS         "moduls"
#define LDB_TBL_NAME_CHANNELS       "channels"
#define LDB_TBL_NAME_DATA           "log_data"
#define LDB_TBL_NAME_SETTINGS       "settings"

#define LDB_TBL_CONTENT_LBUS            "(id integer NOT NULL, tmrecv timestamp, usr text, address text, cmd text, payload text, handled boolean, tmnexthandle timestamp, tmhandled timestamp, PRIMARY KEY(id))"
#define LDB_TBL_CONTENT_USERS           "(id integer NOT NULL, login text, first_name text, last_name text, usergroup text, timezone text, password text, PRIMARY KEY(id))"
#define LDB_TBL_CONTENT_DEVICE_GROUPS   "(id integer NOT NULL, name text, PRIMARY KEY(id))"
#define LDB_TBL_CONTENT_USER_GOUPS      "(id integer NOT NULL, user_group text, delete_devices boolean, register_devices boolean, download boolean, PRIMARY KEY(id))"
#define LDB_TBL_CONTENT_DEVICES         "(id integer NOT NULL, address text, mac text, registered boolean, name text, dev_group text, mint text, pint text, aint text, moduls text, wkc text, PRIMARY KEY(id))"
#define LDB_TBL_CONTENT_MODULS          "(id integer NOT NULL, address text, channels text, mint text, pint text, aint text, PRIMARY KEY(id))"
#define LDB_TBL_CONTENT_CHANNELS        "(id integer NOT NULL, address text, class text, type text, unit text, name text, channel_group text, PRIMARY KEY(id))"
#define LDB_TBL_CONTENT_DATA            "(datetime timestamp NOT NULL, PRIMARY KEY(datetime))"
#define LDB_TBL_CONTENT_SETTINGS        "(name text NOT NULL, value text, PRIMARY KEY(name))"


#define LDB_SQL_SET_NAME_DB_VER           "DB_VER"
#define LDB_SQL_SET_NAME_PANID            "PANID"
#define LDB_SQL_SET_NAME_REGMODE_LEN      "REGMODE_LEN"
#define LDB_SQL_SET_NAME_MAX_REGMODE_LEN  "MAX_REGMODE_LEN"
#define LDB_SQL_SET_NAME_REG_MINT         "REG_MINT"
#define LDB_SQL_SET_NAME_REG_PINT         "REG_PINT"
#define LDB_SQL_SET_NAME_REG_AINT         "REG_AINT"
#define LDB_SQL_SET_NAME_NET_TYPE         "NET_TYPE"
#define LDB_SQL_SET_NAME_NET_ADR          "NET_ADR"
#define LDB_SQL_SET_NAME_NET_MASK         "NET_MASK"
#define LDB_SQL_SET_NAME_NET_GATEWAY      "NET_GATEWAY"



int lielas_deleteTable(char* tblname);
int lielas_createTables();

int lielas_createLbusTbl();
int lielas_createUsersTbl();
int lielas_createDeviceGroupsTbl();
int lielas_createUserGroupsTbl();
int lielas_createDevicesTbl();
int lielas_createModulsTbl();
int lielas_createChannelsTbl();
int lielas_createDataTbl();
int lielas_createSettingsTbl();

int lielas_getLDBSetting(char* dest, const char* name, int maxLen);

#endif




