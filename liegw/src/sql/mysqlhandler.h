#ifndef MYSQLHANDLER_H
#define MYSQLHANDLER_H

#define MSQL_HOST     "localhost"
#define MSQL_USER     "root"
#define MSQL_PASSWORD "lieweb"
#define MSQL_DB       "mini2_th"

#define MSQL_STRING_LEN   200


typedef struct{
  int id;
  char route[MSQL_STRING_LEN];
  char vbat[MSQL_STRING_LEN];
  char bat_state[MSQL_STRING_LEN];
}IGROUTE;

typedef struct{
  int id;
  int route_id;
  char dt[MSQL_STRING_LEN];
  char temperature[MSQL_STRING_LEN];
  char humidity[MSQL_STRING_LEN];
}IGDATASET;


int msql_init();
int msql_get_number_of_routes();
int msql_get_routes(IGROUTE *routes, int maxRoutes);
int msql_get_log_count();
int msql_get_dataset(IGDATASET *ds, int id);

#endif

