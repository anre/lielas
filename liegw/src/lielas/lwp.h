#ifndef LWP_H
#define LWP_H

#include "lielas.h"

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
#define LWP_ATTR_DATABASE_DATETIME      "datetime"
#define LWP_ATTR_CHANNEL_VALUE          "value"
#define LWP_ATTR_CHANNEL_UNIT           "unit"
#define LWP_ATTR_CHANNEL_TYPE            "type"
#define LWP_ATTR_CHANNEL_CLASS          "class"
#define LWP_ATTR_LOGGER_STATE           "state"
#define LWP_ATTR_LOGGER_INTERVAL        "interval"
#define LWP_ATTR_LOGGER_MIN             "min"
#define LWP_ATTR_LOGGER_MAX             "max"
#define LWP_ATTR_LOGGER_STEP            "step"


#define LWP_MAX_ATTRIBUTES      10
#define LWP_MAX_ATTR_NAME_LEN   100
#define LWP_MAX_RES_NAME_LEN    100
#define LWP_MAX_WKC_LEN         2000

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
int lwp_get_attr_value(char *str, lwp_resource *res, char *attr_name, char *val, int size);
#endif


