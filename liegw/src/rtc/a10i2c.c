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

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "a10i2c.h"
#include "../log.h"

static int i2c_adr;
static int i2c_nr;


/********************************************************************************************************************************
 *    void i2c_init(int address, int i2c)
 ********************************************************************************************************************************/
void i2c_init(int address, int i2c){
  i2c_adr = address;
  i2c_nr = i2c;
}

/********************************************************************************************************************************
 *     int i2c_read(unsigned char *buf, int len)
 ********************************************************************************************************************************/
 int i2c_read(uint8_t *buf, int len){
  char cmd[500];
  char bf[100];
  int i;
  FILE *file;
   
  if(i2c_adr == 0){
    lielas_log((uint8_t*)"i2c error: i2c address not set", LOG_LEVEL_WARN);
    return -1;
  }
     
  if(i2c_nr == 0){
    lielas_log((uint8_t*)"i2c error: i2c bus not set", LOG_LEVEL_WARN);
    return -1;
  }
    
  snprintf(cmd, 500, "/usr/sbin/i2cget -y %i %i", i2c_nr, i2c_adr);
    
  for(i=0; i < len; i++){
    
    file = popen(cmd, "r");
    
    if(file == NULL){
      lielas_log((uint8_t*)"i2c error: failed to execute i2cget", LOG_LEVEL_WARN);
      return -1;
    }
    
    if(fgets((char*)bf, 500, file) != NULL){
      buf[i] = (uint8_t)strtol(bf, NULL, 0);
    }
    
    pclose(file);
  }
  return 0;
}
 
 /********************************************************************************************************************************
 *      int i2c_write(usnigned char *buf, int len)
 * 
 *  write len bytes with a byte address
 ********************************************************************************************************************************/
 int i2c_writeb(uint8_t adr, uint8_t *buf, int len){
  char cmd[500];
  int i;
  FILE *file;
   
  if(i2c_adr == 0){
    lielas_log((uint8_t*)"i2c error: i2c address not set", LOG_LEVEL_WARN);
    return -1;
  }
     
  if(i2c_nr == 0){
    lielas_log((uint8_t*)"i2c error: i2c bus not set", LOG_LEVEL_WARN);
    return -1;
  }
  
  
  if(len == 0){
    //only write address, no data
    snprintf(cmd, 500, "/usr/sbin/i2cset -y 0x%x 0x%x 0x%x ", i2c_nr, i2c_adr, adr);
    file = popen(cmd, "r");
      
    if(file == NULL){
      lielas_log((uint8_t*)"i2c error: failed to execute i2cset", LOG_LEVEL_WARN);
      return -1;
    }
      
    pclose(file);
  }else{
    for(i=0; i < len; i++){
      snprintf(cmd, 500, "/usr/sbin/i2cset -y 0x%x 0x%x 0x%x 0x%x", i2c_nr, i2c_adr, (adr+i), buf[i]);
      file = popen(cmd, "r");
        
      if(file == NULL){
        lielas_log((uint8_t*)"i2c error: failed to execute i2cset", LOG_LEVEL_WARN);
        return -1;
      }
        
      pclose(file);
    }
  }
  return 0;
}
 
/*
#include <errno.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
*/

 /*  int adr = 0x68;
  char buf[100] = { 0 };
  int i;
  
  
  int file = open("/dev/i2c-2", O_RDWR);
  if(file < 1){
    exit(1);
  }
  
  
  i = ioctl(file, I2C_SLAVE, adr);
  if(i < 0){
    printf(":( %i: %s\n", errno, strerror(errno));
    
    exit(0);
  }
  
  i = read(file, buf, 2);
  printf("%i bytes\n", i);
    printf(":( %i: %s\n", errno, strerror(errno));
  
  printf("yeah\n");
  exit(0);*/
 
