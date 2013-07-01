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

#ifndef LOG_H
#define LOG_H

#define LOG_LEVEL_ERROR		0
#define LOG_LEVEL_WARN		1
#define LOG_LEVEL_DEBUG		2


#define LOG_TO_STDOUT		0
#define LOG_TO_FILE			1
#define LOG_TO_SYSLOG		2

#define LOG_BUF_LEN     5000

void lielas_log(unsigned char *msg, int level);

#endif
