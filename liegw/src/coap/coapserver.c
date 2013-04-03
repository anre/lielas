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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>

#include "../settings.h"
#include "../coap/libcoap/config.h"
#include "../coap/libcoap/resource.h"
#include "../coap/libcoap/coap.h"
#include "../coap/libcoap/coap_time.h"
#include "../coap/libcoap/encode.h"

static time_t my_clock_base = 0;
static coap_context_t *ctx;

coap_tid_t coapID;

#define COAP_RESOURCE_CHECK_TIME 2

#ifndef min
 	 #define min(a,b) ((a) < (b) ? (a) : (b))
#endif

void hnd_get_index(coap_context_t  *c, struct coap_resource_t *resource,
		           coap_address_t *peer, coap_pdu_t *request, str *token,
		           coap_pdu_t *response){

	unsigned char buf[3];

	response->hdr->code = COAP_RESPONSE_CODE(205);

	coap_add_option(response, COAP_OPTION_CONTENT_TYPE, coap_encode_var_bytes(buf, COAP_MEDIATYPE_TEXT_PLAIN), buf);
	coap_add_option(response, COAP_OPTION_MAXAGE, coap_encode_var_bytes(buf, 0x2ffff), buf);

	if( token->length){
		coap_add_option(response, COAP_OPTION_TOKEN, token->length, token->s);
	}

	coap_add_data(response, strlen("lielas COAP server daemon"), (unsigned char*)"lielas COAP server daemon");

}

void hnd_get_time(coap_context_t  *c, struct coap_resource_t *resource,
        coap_address_t *peer, coap_pdu_t *request, str *token,
        coap_pdu_t *response){

	coap_opt_iterator_t optIter;
	unsigned char buf[5];
	time_t now;

	response->hdr->code = COAP_RESPONSE_CODE(205);

	coap_add_option(response, COAP_OPTION_CONTENT_TYPE, coap_encode_var_bytes(buf, COAP_MEDIATYPE_TEXT_PLAIN), buf);
	coap_add_option(response, COAP_OPTION_MAXAGE, coap_encode_var_bytes(buf, 0x01), buf);

	if(request != NULL && coap_check_option(request, COAP_OPTION_SUBSCRIPTION, &optIter)){
		coap_add_observer(resource, peer, token);
		coap_add_option(response, COAP_OPTION_SUBSCRIPTION, 0, NULL);
	}

	if(resource->dirty == 1){
		coap_add_option(response, COAP_OPTION_SUBSCRIPTION, coap_encode_var_bytes(buf, c->observe), buf);
	}

	if(token->length){
		coap_add_option(response, COAP_OPTION_TOKEN, token->length, token->s);
	}

	time(&now);

	if(request != NULL
			&& coap_check_option(request, COAP_OPTION_URI_QUERY, &optIter)
			&& memcmp(COAP_OPT_VALUE(optIter.option), "ticks", min(5, COAP_OPT_LENGTH(optIter.option))) == 0){
		response->length += snprintf((char*)response->data, response->max_size - response->length, "%u", (unsigned int) now);
	}else{
		struct tm *tmp;
		tmp = gmtime(&now);
		response->length += strftime((char*)response->data, response->max_size - response->length, "%d.%m.%Y %H:%M:%S", tmp);
	}

}

void init_resources(coap_context_t *c){
	coap_resource_t *r;

	r = coap_resource_init(NULL, 0, 0);
	coap_register_handler(r, COAP_REQUEST_GET, hnd_get_index);
	coap_add_attr(r, (unsigned char *) "ct", 2, (unsigned char*) "0", 1, 0);
	coap_add_attr(r, (unsigned char *) "title", 5, (unsigned char *) "\"Lielas\"", 14, 0);
	coap_add_resource(c, r);

	r = coap_resource_init((unsigned char *) "time", 4, 0);
	coap_register_handler(r, COAP_REQUEST_GET, hnd_get_time);
	coap_add_attr(r, (unsigned char *)"ct", 2, (unsigned char*)"0", 1, 0);
	coap_add_attr(r, (unsigned char *)"title", 2, (unsigned char*)"\"clock\"", 16, 0);
	coap_add_attr(r, (unsigned char *)"rt", 2, (unsigned char*)"\"ticks\"", 7, 0);
	coap_add_attr(r, (unsigned char *)"if", 2, (unsigned char*)"\"clock\"", 7, 0);
	coap_add_resource(c, r);


	my_clock_base = clock_offset;
}

coap_context_t *get_context(){
	coap_context_t *c = NULL;
	int s;
	struct addrinfo hints;
	struct addrinfo *result, *rp;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE | AI_NUMERICHOST;

	s = getaddrinfo("::", "5683", &hints, &result);
	if( s!= 0){
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		return NULL;
	}

	/* iterate through results until success */
	for(rp = result; rp!= NULL; rp = rp->ai_next){
		coap_address_t addr;
		if(rp->ai_addrlen <= sizeof(addr.addr)){
			coap_address_init(&addr);
			addr.size = rp->ai_addrlen;
			memcpy(&addr.addr, rp->ai_addr, rp->ai_addrlen);

			c = coap_new_context(&addr);
			if(c){

				freeaddrinfo(result);
				return c;
			}
		}
	}

	freeaddrinfo(result);
	return c;
}

void COAPfreeContext(){
	coap_free_context( ctx );
}

int COAPinit(){
	coap_set_log_level(LOG_WARN);

	ctx = get_context();
	if(!ctx){
		return -1;
	}

	coap_clock_init();
	init_resources(ctx);

	atexit(COAPfreeContext);

	return 0;
}

void COAPhandleServer(){
	fd_set readfds;
	coap_queue_t *nextpdu;
	coap_tick_t now;
	struct timeval tv = {1, 0};
	struct timeval *timeout;
	int result;

	FD_ZERO(&readfds);
	FD_SET( ctx->sockfd, &readfds);

	nextpdu = coap_peek_next(ctx);

	coap_ticks(&now);
	while( nextpdu && nextpdu->t <= now){
		coap_retransmit( ctx, coap_pop_next(ctx));
		nextpdu = coap_peek_next(ctx);
	}



	if( nextpdu && nextpdu->t <= now + COAP_RESOURCE_CHECK_TIME){
		tv.tv_usec = ((nextpdu->t - now) % COAP_TICKS_PER_SECOND) * 1000000 / COAP_TICKS_PER_SECOND;
		tv.tv_sec = (nextpdu->t - now) / COAP_TICKS_PER_SECOND;
		timeout = &tv;
	}else{
		tv.tv_usec = 0;
		tv.tv_sec = COAP_RESOURCE_CHECK_TIME;
		timeout = &tv;
	}
	result = select( FD_SETSIZE, &readfds, 0, 0, timeout);


	if( result < 0){	/* error */
		printf("Error: %s\n", strerror(errno));
	}else if( result > 0){	/* read from socket */
		if( FD_ISSET( ctx->sockfd, &readfds )){
			coap_read( ctx );
			coap_dispatch( ctx );
		}
	}else{	/* timeout */

	}


}

coap_tid_t COAPgetID(){
	if(coapID > 30000)
		coapID = 1;
	return coapID++;
}











