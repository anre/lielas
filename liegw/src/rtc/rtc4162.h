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

#ifndef RTC4162_H
#define RTC4162_H

#include <time.h>

#define RTC4162_ADDRESS         0x68
#define RTC4162_I2C_INTERFACE   1     //i2c-1 on cubieboard

#define RTC4162_REG_MSECOND 0x00
#define RTC4162_REG_SECOND  0x01
#define RTC4162_REG_MINUTE  0x02
#define RTC4162_REG_HOUR    0x03
#define RTC4162_REG_DAYOW   0x04
#define RTC4162_REG_DAY     0x05
#define RTC4162_REG_MONTH   0x06
#define RTC4162_REG_YEAR    0x07
#define RTC4162_REG_CALIB   0x08

#define RTC4162_FLAG_ST       7
#define RTC4162_FLAG_AFE      7
#define RTC4162_FLAG_SQWE     6

#define RTC4162_REG_MONTH_ALARM  0x0A
#define RTC4162_REG_DAY_ALARM    0x0B
#define RTC4162_REG_HOUR_ALARM   0x0C
#define RTC4162_REG_MINUTE_ALARM 0x0D
#define RTC4162_REG_SECOND_ALARM 0x0E

#define RTC4162_REG_FLAGS   0x0F
#define RTC4162_FLAG_OF     2
#define RTC4162_FLAG_AF     6

#define RTC4162_MASK_MSEC   0xFF
#define RTC4162_MASK_SEC    0x7F
#define RTC4162_MASK_MIN    0x7F
#define RTC4162_MASK_HOUR   0x3F
#define RTC4162_MASK_DAYOW  0x07
#define RTC4162_MASK_DAYOM  0x3F
#define RTC4162_MASK_MONTH  0x1F
#define RTC4162_MASK_YEAR   0xFF

#define RTC4162_YEAR_OFFSET 100

#define BYTE2BCD(x) (((((x)/10)<<4) & 0xF0) + (x)%10)
#define BCD2BYTE(x) ((((x)>>4) & 0x0F)*10 + ((x) & 0x0F))
#ifndef BV
  #define BV(x) (1<<(x))
#endif

#define RTC4162_OSC_RESET 2
#define RTC4162_OK 0
#define RTC4162_ERROR -1

#define RTC4162_REGISTERS 16

int rtc4162_init();
int rtc4162_get(struct tm *dt);
int rtc4162_set(struct tm *dt);

#endif

