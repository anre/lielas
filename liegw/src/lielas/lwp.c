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
#include <stdlib.h>
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
int lwp_get_attr_value(char *attr, int attrSize, char *val, int size, int pos){
  int i;
  int start = 0;
  int end = 0;
  
  if( pos < 1){
    return -1;
  }
  pos -= 1;
  
  //search for start
  for(i = 0; attr[i] != 0 && i < attrSize; i++){
    if(attr[i] == '\t' || attr[i] == 0 || i == (attrSize -1)){
      pos -= 1;
      i += 1;
    }
    
    
    if(pos == 0){
      //found value
      start = i;
      break;
    }
  }
  if(i == (attrSize -1) && pos != 0){
    return -1;
  }
  
  //search for end
  for(; i < attrSize; i++){
    if(attr[i] == '\t' || attr[i] == 0 || i == (attrSize -1)){
      //found end
      end = i;
      break;
    }
    if(attr[i] == 0){
      return -1;
    }
  }
  
  //copy string
  for(i = 0; i < (end - start) && i < size; i++){
    val[i] = attr[start + i];
  }
  
  if( i >= size){
    return -1;
  }
  val[i] = 0;
  return 0;
}

/********************************************************************************************************************************
 * 		int lwp_get_vbat(char* attr, int len)
 ********************************************************************************************************************************/ 
int lwp_get_vbat(char* attr, int len){
  int ival;
  double val = strtod(&attr[5], NULL);
  if(val < 2. ||val > 4.){
    val = 0.0;
  }
  ival = (int)(val * 1000);
  return ival;
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
 
 
 
 
 
 
 
 
 
 
 
 
 
