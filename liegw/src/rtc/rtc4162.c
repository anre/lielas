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
#include <unistd.h>

#include "rtc4162.h"
#include "a10i2c.h"
#include "../log.h"

static int oscfailed;

int read_register(uint8_t reg, uint8_t *data, int len);
int write_register(uint8_t reg, uint8_t *data, int len);

/********************************************************************************************************************************
 *     void rtc_init()
 ********************************************************************************************************************************/
int rtc4162_init(){
  uint8_t byte;
  uint8_t status;
  
  i2c_init(RTC4162_ADDRESS, 1);
  
  status = read_register(RTC4162_REG_FLAGS, &byte, 1);
  
  if(status){
    lielas_log((uint8_t *)"rtc error: failed to read REG_FLAGS", LOG_LEVEL_ERROR);
    return -1;
  }
  
  oscfailed = 0;
  
  if(byte & BV(RTC4162_FLAG_OF)){
    oscfailed = 1;
    lielas_log((uint8_t *)"rtc: oscillator failure or initial startup", LOG_LEVEL_WARN);
		// oscillator failed -> reset oscillator
    byte |= BV(RTC4162_FLAG_ST);
    status |= write_register(RTC4162_REG_SECOND, &byte, 1);
		byte &= ~BV(RTC4162_FLAG_ST);
		status |= write_register(RTC4162_REG_SECOND, &byte, 1);
    
    if(status){
      lielas_log((uint8_t *)"rtc error: failed to reset oscillator", LOG_LEVEL_ERROR);
      return -1;
    }
    
    
    //wait for oscillator
    sleep(5);
    
		// disable 32.768 kHz square wave output
		status = read_register(RTC4162_REG_DAYOW, &byte, 1);
    if(status){
      lielas_log((uint8_t *)"rtc error: failed to read REG_DAYOW", LOG_LEVEL_ERROR);
      return -1;
    }    
		byte &= 0x0F;
		status = write_register(RTC4162_REG_DAYOW, &byte, 1);
    if(status){
      lielas_log((uint8_t *)"rtc error: failed to write REG_DAYOW", LOG_LEVEL_ERROR);
      return -1;
    }  
    
		status = read_register(RTC4162_REG_MONTH_ALARM, &byte, 1);
    if(status){
      lielas_log((uint8_t *)"rtc error: failed to read REG_MONTH_ALARM", LOG_LEVEL_ERROR);
      return -1;
    }    
		byte &= ~BV(RTC4162_FLAG_SQWE); 
		status = write_register(RTC4162_REG_MONTH_ALARM, &byte, 1);
    if(status){
      lielas_log((uint8_t *)"rtc error: failed to write REG_MONTH_ALARM", LOG_LEVEL_ERROR);
      return -1;
    }  
  }
  return 0;
}

/********************************************************************************************************************************
 *     int rtc_read(struct tm *dt)
 ********************************************************************************************************************************/
int rtc4162_get(struct tm *dt){
 
  uint8_t buf[20];
  
  buf[0] = 0;
  
  if(oscfailed){
    lielas_log((uint8_t *)"rtc error: failed to read date/time, oscfailed flag is set", LOG_LEVEL_ERROR);
    return -1;
  }
  
  if(read_register(RTC4162_REG_MSECOND, buf, 8)){
    lielas_log((uint8_t *)"rtc error: failed to read date/time", LOG_LEVEL_ERROR);
    return -1;
  }
  
  dt->tm_sec = BCD2BYTE(buf[1] & RTC4162_MASK_SEC); 
  dt->tm_min = BCD2BYTE(buf[2] & RTC4162_MASK_MIN);
  dt->tm_hour= BCD2BYTE(buf[3] & RTC4162_MASK_HOUR);
  dt->tm_mday= BCD2BYTE(buf[5] & RTC4162_MASK_DAYOM);
  dt->tm_mon = BCD2BYTE(buf[6] & RTC4162_MASK_MONTH);
  dt->tm_year= BCD2BYTE(buf[7] & RTC4162_MASK_YEAR) + RTC4162_YEAR_OFFSET;
  dt->tm_wday = 0;
  dt->tm_yday = 0;
  dt->tm_isdst = 0;
  
  return 0;
}

/********************************************************************************************************************************
 *     int rtc_set(struct tm *dt)
 ********************************************************************************************************************************/
int rtc4162_set(struct tm *dt){
  uint8_t buf[10] = {0};
  
  buf[1] = (buf[1] & ~RTC4162_MASK_SEC) | (BYTE2BCD(dt->tm_sec) & RTC4162_MASK_SEC);
	buf[2] = (buf[2] & ~RTC4162_MASK_MIN) | (BYTE2BCD(dt->tm_min) & RTC4162_MASK_MIN);
	buf[3] = (buf[3] & ~RTC4162_MASK_HOUR) | (BYTE2BCD(dt->tm_hour) & RTC4162_MASK_HOUR);
	
	buf[5] = (buf[5] & ~RTC4162_MASK_DAYOM) | (BYTE2BCD(dt->tm_mday) & RTC4162_MASK_DAYOM);
	buf[6] = (buf[6] & ~RTC4162_MASK_MONTH) | (BYTE2BCD(dt->tm_mon) & RTC4162_MASK_MONTH);
	buf[7] = (buf[7] & ~RTC4162_MASK_YEAR) | (BYTE2BCD((uint8_t)(dt->tm_year - RTC4162_YEAR_OFFSET)) & RTC4162_MASK_YEAR);
  
  if(write_register(RTC4162_REG_MSECOND, buf, 8)){
    lielas_log((uint8_t *)"rtc error: failed to set date/time", LOG_LEVEL_ERROR);
    return -1;
  }
  
  //clear oscfail bit
  if(oscfailed){
    buf[0] = 0;
    if(read_register(RTC4162_REG_FLAGS, buf, 1)){
      lielas_log((uint8_t *)"rtc error: failed to read REG_FLAGS", LOG_LEVEL_ERROR);
      return -1;
    }
    buf[0] &= ~BV(RTC4162_FLAG_OF);
    if(write_register(RTC4162_REG_FLAGS, buf, 1)){
      lielas_log((uint8_t *)"rtc error: failed to write REG_FLAGS", LOG_LEVEL_ERROR);
      return -1;
    }
  }
  return 0;
}



/********************************************************************************************************************************
 *     int read_register(uint8_t reg, uint8_t *data, int len)
 ********************************************************************************************************************************/
int read_register(uint8_t reg, uint8_t *data, int len){
  
  if(i2c_writeb(reg, &reg, 0))
    return -1;
    
  return i2c_read(data, len);
}

/********************************************************************************************************************************
 *     int write_register(uint8_t reg, uint8_t *data, int len)
 ********************************************************************************************************************************/
int write_register(uint8_t reg, uint8_t *data, int len){
  
  if(len > RTC4162_REGISTERS){
    lielas_log((uint8_t *)"rtc write_register error: len to big", LOG_LEVEL_WARN);
    return -1;
  }

  return i2c_writeb(reg, data, len);
}

