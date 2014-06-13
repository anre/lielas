#define MYSQL_CLIENT_NO_THREADS

#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mysqlhandler.h"

  static MYSQL *con = NULL;

  
/********************************************************************************************************************************
 * 		int msql_init()
 *  
 *  connect to mySQL
 ********************************************************************************************************************************/  
int msql_init(){
  
  con = mysql_init(NULL);
  if(con == NULL){
    fprintf(stderr, "%s\n", mysql_error(con));
    printf("can't connect\n");
    return -1;
  }
  
  if(mysql_real_connect(con, MSQL_HOST, MSQL_USER, MSQL_PASSWORD,
      MSQL_DB, 0, NULL, 0) == NULL){
    printf("can't realy connect: %s\n", mysql_error(con));
    mysql_close(con);
    return -1;
  }

  return 0;
}

/********************************************************************************************************************************
 * 		int msql_get_number_of_routes()
 *  
 *  returns the number of active routes
 ********************************************************************************************************************************/ 
int msql_get_number_of_routes(){
  MYSQL_RES *res;

  if(con == NULL){
    return -1;
  }
  
  if(mysql_query(con, "SELECT route_id FROM routes")){
    return -1;
  }
  
  res = mysql_store_result(con);
  if(res == NULL){
    return -1;
  }
  
  return mysql_num_rows(res);
}

/********************************************************************************************************************************
 * 		int msql_get_number_of_routes()
 *  
 *  returns a list of active routes
 ********************************************************************************************************************************/ 
int msql_get_routes(IGROUTE *routes, int maxRoutes){
  MYSQL_RES *res;
  MYSQL_ROW row;
  int count = 0;

  if(con == NULL){
    return -1;
  }
  
  if(mysql_query(con, "SELECT route_id, route, bat_volt, bat_state FROM routes")){
    return -1;
  }
  
  res = mysql_store_result(con);
  
  while((row = mysql_fetch_row(res)) != NULL){
    routes[count].id = strtol(row[0], NULL, 10);
    strncpy(routes[count].route, row[1], MSQL_STRING_LEN);
    strncpy(routes[count].vbat, row[2], MSQL_STRING_LEN);
    strncpy(routes[count].bat_state, row[3], MSQL_STRING_LEN);
    count += 1;
  }
  return count;
}

/********************************************************************************************************************************
 * 		int msql_get_log_count()
 *  
 *  returns the number of logs
 ********************************************************************************************************************************/ 
int msql_get_log_count(){
  MYSQL_RES *res;
  MYSQL_ROW row;
  
  
  if(con == NULL){
    return 0;
  }
  
  if(mysql_query(con, "SELECT id FROM log_data ORDER BY id DESC LIMIT 1")){
    return 0;
  }
  
  res = mysql_store_result(con);
  
  if((row = mysql_fetch_row(res)) == NULL){
    return 0;
  }
  
  return strtol(row[0], NULL, 10);
}

/********************************************************************************************************************************
 * 		int msql_get_dataset(IGDATASET *ds, int id)
 *  
 *  saves dataset with given id in ds when everything is right
 *  else returns -1
 ********************************************************************************************************************************/ 
int msql_get_dataset(IGDATASET *ds, int id){
  MYSQL_RES *res;
  MYSQL_ROW row;
  char query[500];

  if(con == NULL){
    return -1;
  }
  
  snprintf(query, sizeof(query), "SELECT id, route_id, dt, temperature, humidity FROM log_data WHERE id=%i", id);
  if(mysql_query(con, query)){
    return -1;
  }
  
  res = mysql_store_result(con);
  if((row = mysql_fetch_row(res)) == NULL){
    return -1;
  }
  ds->id = id;
  ds->route_id = strtol(row[1], NULL, 10);
  strncpy(ds->dt, row[2], MSQL_STRING_LEN);
  strncpy(ds->temperature, row[3], MSQL_STRING_LEN);
  strncpy(ds->humidity, row[4], MSQL_STRING_LEN);
  
  return 0;
}










