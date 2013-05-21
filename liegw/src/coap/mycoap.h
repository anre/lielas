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

#ifndef MYCOAP_H
#define MYCOAP_H

#include "libcoap/coap.h"

#define MYCOAP_METHOD_GET		1
#define MYCOAP_METHOD_POST		2
#define MYCOAP_METHOD_PUT		3
#define MYCOAP_METHOD_DELETE	4

#define MYCOAP_TRIES  3

#define COAP_STATUS_CONTENT	69

int order_opts(void *a, void *b) ;
void message_handler(struct coap_context_t  *ctx,
		const coap_address_t *remote,
		coap_pdu_t *sent,
		coap_pdu_t *received,
		const coap_tid_t id);

typedef struct {
	char *buf;
	int status;
	int len;
  int bufSize;
} coap_buf;


int coap_send_cmd(char* uriStr, coap_buf *cb, unsigned char methode, unsigned char *post);

coap_buf *coap_create_buf();
void coap_delete_buf(coap_buf *cb);

extern int check_segment(const unsigned char *s, size_t length);
extern void decode_segment(const unsigned char *seg, size_t length, unsigned char *buf);



#endif
