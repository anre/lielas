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

#define LDB_TBL_CONTENT_LBUS            "(id integer NOT NULL, tmrecv timestamp, usr text, address text, cmd text, payload text, handled boolean, tmhandled timestamp, PRIMARY KEY(id))"
#define LDB_TBL_CONTENT_USERS           "(id integer NOT NULL, login text, first_name text, last_name text, usergroup text, timezone text, password text, PRIMARY KEY(id))"
#define LDB_TBL_CONTENT_DEVICE_GROUPS   "(id integer NOT NULL, name text, PRIMARY KEY(id))"
#define LDB_TBL_CONTENT_USER_GOUPS      "(id integer NOT NULL, user_group text, delete_devices boolean, register_devices boolean, download boolean, PRIMARY KEY(id))"
#define LDB_TBL_CONTENT_DEVICES         "(id integer NOT NULL, address text, mac text, registered boolean, name text, dev_group text, mint text, pint text, aint text, moduls text, wkc text, PRIMARY KEY(id))"
#define LDB_TBL_CONTENT_MODULS          "(id integer NOT NULL, address text, channels text, mint text, pint text, aint text, PRIMARY KEY(id))"
#define LDB_TBL_CONTENT_CHANNELS        "(id integer NOT NULL, address text, class text, type text, unit text, name text, channel_group text, PRIMARY KEY(id))"
#define LDB_TBL_CONTENT_DATA            "(datetime timestamp NOT NULL, PRIMARY KEY(datetime))"
#define LDB_TBL_CONTENT_SETTINGS        "(name text NOT NULL, value text, PRIMARY KEY(name))"


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

#endif




