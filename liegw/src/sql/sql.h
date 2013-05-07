/*
*
*	Copyright (c) 2013 Andreas Reder
*	Author      : Andreas Reder <andreas@reder.eu>
*	File		: 
*
*	This File is part of lieweb, a web-gui for osd-compatible devices
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

#ifndef SQL_H_
#define SQL_H_


#include "libpq-fe.h"

//#include <libpq-fe.h>
#define SQL_BUFFER_SIZE	200
#define SQL_STATEMENT_BUF_SIZE  1000

int SQLconnect();
void SQLclose();

PGresult *SQLexec(char *cmd);
int SQLTableExists(char *name);
int SQLRowExists( char* table, char* row);
int SQLCellExists(char* table, char* column, char* val);

#endif /* SQL_H_ */
