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

#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "lwp.h"
#include "../log.h"


/********************************************************************************************************************************
 * 		lwp_init_wkc
 ********************************************************************************************************************************/
void lwp_init_wkc(lwp_wkc *wkc){
  lwp_init_resource(&wkc->info);
  lwp_init_resource(&wkc->device);
  lwp_init_resource(&wkc->modul);
  lwp_init_resource(&wkc->network);
  lwp_init_resource(&wkc->database);
  lwp_init_resource(&wkc->logger);
  wkc->channels = 0;
}

/********************************************************************************************************************************
 * 		lwp_init_resource
 ********************************************************************************************************************************/
void lwp_init_resource(lwp_resource *res){
  res->name[0] = 0;
  res->attributes = 0;
}

/********************************************************************************************************************************
 * 		lwp_init_attr
 ********************************************************************************************************************************/
void lwp_init_attr(lwp_attr *attr){
  attr->name[0] = 0;
}

/********************************************************************************************************************************
 * 		lwp_parse_wkc
 ********************************************************************************************************************************/
int lwp_parse_wkc(char *str, lwp_wkc *wkc){
  char resname[LWP_MAX_RES_NAME_LEN];
  char attrname[LWP_MAX_ATTR_NAME_LEN];
  char *title;
  char log[1000];
  int pos = 0;
  int eof = 0;
  int eoa = 0;
  int len;
  int i;
  lwp_resource *res;
  //char wkcstr[LWP_MAX_WKC_LEN];
  
  
  if(str[0] == 0){
    lielas_log((unsigned char*)"error parsing wkc, empty wkc string", LOG_LEVEL_WARN);
    return -1;
  }
  
  //init wkc
  wkc->channels = 0;
  lwp_init_resource(&wkc->info);
  lwp_init_resource(&wkc->device);
  lwp_init_resource(&wkc->modul);
  lwp_init_resource(&wkc->network);
  lwp_init_resource(&wkc->database);
  lwp_init_resource(&wkc->logger);
  for(i = 0; i < MAX_CHANNELS; i++){
    lwp_init_resource(&wkc->channel[i]);
  }
  
  str = strchr(&str[1], '<');
  len = strlen(str);
  while(!eof){
    //search resource token
    if(str[pos] == '<' && str[pos+1] == '/'){
      pos += 2;
      //get resource name
      for(i = 0; i < LWP_MAX_RES_NAME_LEN && ((pos+i) < len); i++){
        if(str[pos+i] == '>'){
          break;
        }
        resname[i] = str[pos+i];
      }
      resname[i] = 0;
      res = lwp_get_res_by_name(resname, wkc);
      
      //special case resource channel
      if(strncmp(resname, LWP_RESOURCE_CHANNEL, strlen(LWP_RESOURCE_CHANNEL)) == 0){
        wkc->channels += 1;
        if( wkc->channels >= MAX_CHANNELS){
          wkc->channels -= 1;
          res = 0;
        }
      }
        
      if(res == 0){ //unknown resource
        snprintf(log, 1000, "unknown resource parsing .well-known/core: %s", resname);
        lielas_log((unsigned char*)log, LOG_LEVEL_WARN);
      }else{  // save resource and parse attributes
        strncpy(res->name, resname, LWP_MAX_RES_NAME_LEN);
      
        //search title token
        title = strstr(&str[pos], "title");
        if(title == NULL){
          lielas_log((unsigned char*)"error parsing wkc, title token not found", LOG_LEVEL_WARN);
          return -1;
        }
        //parse title
        title = strchr(title, '"');
        if(title == NULL){
          lielas_log((unsigned char*)"error parsing wkc, title value not found", LOG_LEVEL_WARN);
          return -1;
        }
        
        pos = title - str + 1;
        eoa = 0;
        
        i = 0;
        while(!eoa){  // scan full title
          for(i = 0; i < LWP_MAX_ATTR_NAME_LEN && ((pos+i) < len); i++){
            if( str[pos+i] == ' '){
              break;
            }else if(str[pos+i] == '"' || str[pos+i] == ' ' || str[pos+i] == '\n' || str[pos+i] == '|'){ // end of title
              eoa = 1;
              break;
            }
            attrname[i] =  str[pos+i];
          }
          attrname[i] = 0;
          pos += i + 1;
          if( strlen(attrname) > 0){
            if(lwp_add_attr(attrname, res) != 0){
              lielas_log((unsigned char*)"error parsing wkc, can't add attribute", LOG_LEVEL_WARN);
              return -1;
            }
          }
        }
      } 
    }
      
    if(str[pos] == 0 || pos > len){
      eof = 1;
    }
    pos += 1;
  }
  
  //lwp_print_wkc(wkc, wkcstr);
  //lielas_log((unsigned char*) wkcstr, LOG_LEVEL_DEBUG);
  
  return 0;
}

/********************************************************************************************************************************
 * 		lwp_get_res_by_name
 ********************************************************************************************************************************/
lwp_resource *lwp_get_res_by_name( char* str, lwp_wkc *wkc){
  lwp_resource *res = 0;
  
  if(!strncmp(str, LWP_RESOURCE_CHANNEL, strlen(LWP_RESOURCE_CHANNEL))){
    res = &wkc->channel[wkc->channels+1];
  }else if(!strcmp(str, LWP_RESOURCE_DATABASE)){
    res = &wkc->database;
  }else if(!strcmp(str, LWP_RESOURCE_DEVICE)){
    res = &wkc->device;
  }else if(!strcmp(str, LWP_RESOURCE_INFO)){
    res = &wkc->info;
  }else if(!strcmp(str, LWP_RESOURCE_LOGGER)){
    res = &wkc->logger;
  }else if(!strcmp(str, LWP_RESOURCE_MODUL)){
    res = &wkc->modul;
  }else if(!strcmp(str, LWP_RESOURCE_NETWORK)){
    res = &wkc->network;
  }
  return res;
}
/********************************************************************************************************************************
 * 		lwp_add_attr
 ********************************************************************************************************************************/
int lwp_add_attr(char* name, lwp_resource *res){
  res->attributes += 1;
  if(res->attributes >= LWP_MAX_ATTRIBUTES){
    res->attributes -= 1;
    return -1;
  }
  
  strncpy(res->attr[res->attributes -1].name, name, LWP_MAX_ATTR_NAME_LEN);
  return 0;
}

/********************************************************************************************************************************
 * 		lwp_add_attr
 ********************************************************************************************************************************/
void lwp_print_wkc(lwp_wkc *wkc, char* str){
  int i, j;
  int pos = 0;
  lwp_resource *res;
  
  pos += snprintf(&str[pos], LWP_MAX_WKC_LEN - pos, ".well-known/core:\n");
  res = &wkc->info;
  pos += snprintf(&str[pos], LWP_MAX_WKC_LEN - pos, "Resource %s:\n", res->name);
  for(i = 0; i < res->attributes; i++){
    pos += snprintf(&str[pos], LWP_MAX_WKC_LEN - pos, "\t Attribute %s with rank %i of %i\n", res->attr[i].name, i, res->attributes);
  }    
  res = &wkc->device;
  pos += snprintf(&str[pos], LWP_MAX_WKC_LEN - pos, "Resource %s:\n", res->name);
  for(i = 0; i < res->attributes; i++){
    pos += snprintf(&str[pos], LWP_MAX_WKC_LEN - pos, "\t Attribute %s with rank %i of %i\n", res->attr[i].name, i, res->attributes);
  }   
  res = &wkc->modul;
  pos += snprintf(&str[pos], LWP_MAX_WKC_LEN - pos, "Resource %s:\n", res->name);
  for(i = 0; i < res->attributes; i++){
    pos += snprintf(&str[pos], LWP_MAX_WKC_LEN - pos, "\t Attribute %s with rank %i of %i\n", res->attr[i].name, i, res->attributes);
  }   
  res = &wkc->network;
  pos += snprintf(&str[pos], LWP_MAX_WKC_LEN - pos, "Resource %s:\n", res->name);
  for(i = 0; i < res->attributes; i++){
    pos += snprintf(&str[pos], LWP_MAX_WKC_LEN - pos, "\t Attribute %s with rank %i of %i\n", res->attr[i].name, i, res->attributes);
  }   
  res = &wkc->database;
  pos += snprintf(&str[pos], LWP_MAX_WKC_LEN - pos, "Resource %s:\n", res->name);
  for(i = 0; i < res->attributes; i++){
    pos += snprintf(&str[pos], LWP_MAX_WKC_LEN - pos, "\t Attribute %s with rank %i of %i\n", res->attr[i].name, i, res->attributes);
  }
  for(j = 1; j <= wkc->channels && j < MAX_CHANNELS; j++){
    res = &wkc->channel[j];
    pos += snprintf(&str[pos], LWP_MAX_WKC_LEN - pos, "Resource %s:\n", res->name);
    for(i = 0; i < res->attributes; i++){
      pos += snprintf(&str[pos], LWP_MAX_WKC_LEN - pos, "\t Attribute %s with rank %i of %i\n", res->attr[i].name, i, res->attributes);
    }
  }
  res = &wkc->logger;
  pos += snprintf(&str[pos], LWP_MAX_WKC_LEN - pos, "Resource %s:\n", res->name);
  for(i = 0; i < res->attributes; i++){
    pos += snprintf(&str[pos], LWP_MAX_WKC_LEN - pos, "\t Attribute %s with rank %i of %i\n", res->attr[i].name, i, res->attributes);
  } 
}

/********************************************************************************************************************************
 * 		lwp_parse_resource
 ********************************************************************************************************************************/ 
int lwp_get_attr_value(char *str, lwp_resource *res, char *attr_name, char *val, int size){
  int rank;
  int pos =0 ;
  int i;

  //get attriubte rank
  for(rank=0; rank < res->attributes && rank < LWP_MAX_ATTRIBUTES; rank++){
    if(strcmp(res->attr[rank].name, attr_name)== 0){
      break;
    }
  }
  if(rank >= LWP_MAX_ATTRIBUTES || rank >= res->attributes){
    return -1;
  }
  
  while(rank > 0){
    if(str[pos] == '\t' || str[pos] == '\n' || str[pos] == 0){
      rank -= 1;
    }
    pos += 1;
    if(pos >= size){
      return -1;
    }
  }
  for( i = 0; pos < size; i++){
    if(str[pos+i] == '\t' || str[pos+i] == '\n' || str[pos+i] == 0){
      break;
    }
    val[i] = str[pos + i];
  }
  
  if((pos+i) >= size){
    val[0] = 0;
    return -1;
  }
  val[i] = 0;
  return 0;
}

/********************************************************************************************************************************
 * 		lwp_compdt_to_struct_tm
 * 
 *  convert compressed dt to struct tm
 ********************************************************************************************************************************/  
void lwp_compdt_to_struct_tm(uint8_t *cdt, struct tm *dt){
  dt->tm_sec=(cdt[3]&0xFC)>>2;
  dt->tm_min=((cdt[2]&0xF0)>>4)+((cdt[3]&0x03)<<4);
  dt->tm_hour=((cdt[1]&0x80)>>7)+((cdt[2]&0x0F)<<1);
  dt->tm_mday=(cdt[1]&0x7C)>>2;
  dt->tm_mon=((cdt[0]&0xC0)>>6)+((cdt[1]&0x03)<<2)-1;
  dt->tm_year=(cdt[0]&0x3f)+LWP_CENTURY_OFFSET;
  dt->tm_wday=0;
  dt->tm_yday=0;
  dt->tm_isdst=0;
}

/********************************************************************************************************************************
 * 		lwp_macto std_mac
 * 
 *  convert lwp mac to std mac format string
 ********************************************************************************************************************************/  
void lwp_mac_to_std_mac(char* dest, char* src){
  dest[0] = toupper(src[0]);
  dest[1] = toupper(src[1]);
  dest[2] = ':';
  dest[3] = toupper(src[2]);
  dest[4] = toupper(src[3]);
  dest[5] = ':';
  dest[6] = toupper(src[4]);
  dest[7] = toupper(src[5]);
  dest[8] = ':';
  dest[9] = toupper(src[6]);
  dest[10] = toupper(src[7]);
  dest[11] = ':';
  dest[12] = toupper(src[8]);
  dest[13] = toupper(src[9]);
  dest[14] = ':';
  dest[15] = toupper(src[10]);
  dest[16] = toupper(src[11]);
  dest[17] = ':';
  dest[18] = toupper(src[12]);
  dest[19] = toupper(src[13]);
  dest[20] = ':';
  dest[21] = toupper(src[14]);
  dest[22] = toupper(src[15]);
  dest[23] = 0; 
}
 
 
 
 
 
 
 
 
 
 
 
 
 
