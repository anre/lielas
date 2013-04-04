#ifndef LBUS_H
#define LBUS_H

#include <time.h>

#define LBUF_MAX_ADDRESS_SIZE	   100
#define LBUF_MAX_CMD_SIZE		    1000
#define LBUF_MAX_PAYLOAD_SIZE		1000

typedef struct{
	long id;
	struct tm date;
	char address[LBUF_MAX_ADDRESS_SIZE];
	char cmd[LBUF_MAX_CMD_SIZE];
  char payload[LBUF_MAX_PAYLOAD_SIZE];
	int handled;
	struct tm handledTm;
} Lbuscmd;

typedef struct Lbuscontainer_struct{
	Lbuscmd *lcmd;
	struct Lbuscontainer_struct *llbc;	// last lbus container
	struct Lbuscontainer_struct *nlbc;	// next lbus container
} Lbuscontainer;

int lbus_init();
int lbus_add(Lbuscmd *cmd);
int lbus_remove(Lbuscmd *cmd);
Lbuscmd *lbus_getFirstCmd();
Lbuscmd *lbus_getNextCmd();

#endif

