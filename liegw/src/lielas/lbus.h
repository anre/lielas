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

#ifndef LBUS_H
#define LBUS_H

#include <time.h>

#define LBUF_MAX_ADDRESS_SIZE	   100
#define LBUF_MAX_USER_SIZE       200
#define LBUF_MAX_CMD_SIZE		    1000
#define LBUF_MAX_PAYLOAD_SIZE		1000
#define LBUF_MAX_ERR_STR_SIZE    500

#define LBUS_TOK_ADDRESS	"address"
#define LBUS_TOK_USER     "user"
#define LBUS_TOK_CMD			"cmd"
#define LBUS_TOK_PAYLOAD	"payload"
#define LBUS_TOK_DEVICE   "device"
#define LBUS_TOK_RUNMODE  "runmode"
#define LBUS_TOK_NET_TYPE "net_type"
#define LBUS_TOK_NET_ADR  "net_address"
#define LBUS_TOK_NET_MASK "net_mask"
#define LBUS_TOK_NET_GW   "net_gateway"

#define LBUS_CMD_CHG          "chg"
#define LBUS_CMD_DEL          "del"
#define LBUS_CMD_LOGIN        "login"

#define LBUS_PAYLOAD_DEL_DATA "data"
#define LBUS_PAYLOAD_DEL_DB   "db"
#define LBUS_PAYLOAD_RUNMODE_NORMAL   "normal"
#define LBUS_PAYLOAD_RUNMODE_REGISTER "register"

#define LBUS_ADDRESS_LIEGW    "liegw"

#define LBUS_BUF_SIZE          200
#define LBUS_SQL_BUFFER_SIZE	1000


#define LBUS_NET_RELOGIN_TIME   60


typedef struct{
	long id;
	struct tm tmrecv;
	char address[LBUF_MAX_ADDRESS_SIZE];
  char user[LBUF_MAX_USER_SIZE];
	char cmd[LBUF_MAX_CMD_SIZE];
  char payload[LBUF_MAX_PAYLOAD_SIZE];
	int handled;
  struct tm *tmnexthandle;
	struct tm tmhandled;
} Lbuscmd;

typedef struct Lbuscontainer_struct{
	Lbuscmd *lcmd;
	struct Lbuscontainer_struct *llbc;	// last lbus container
	struct Lbuscontainer_struct *nlbc;	// next lbus container
} Lbuscontainer;

int lbus_init();
int lbus_add(Lbuscmd *cmd, int saveToDb);
int lbus_load();
Lbuscmd *lbus_createCmd(int id);
void lbus_deleteCmd(Lbuscmd *cmd);
void lbus_remove(Lbuscmd *cmd);
Lbuscmd *lbus_getFirstCmd();
Lbuscmd *lbus_getNextCmd();
void lbus_handler();
int lbus_printCmd(char *s, int maxLen, Lbuscmd *cmd);

#endif

