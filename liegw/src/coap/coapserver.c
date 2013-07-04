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
#include "../lielas/lbus.h"
#include "../jsmn/jsmn.h"
#include "../log.h"
#include "../devicehandler.h"
#include "../rtc/rtc.h"

static time_t my_clock_base = 0;
static coap_context_t *ctx;

coap_tid_t coapID;

#define COAP_RESOURCE_CHECK_TIME 2

#define MAX_PAYLOAD_SIZE 1000

#ifndef min
 	 #define min(a,b) ((a) < (b) ? (a) : (b))
#endif

/********************************************************************************************************************************
 * 		hnd_get_index: index resource get handler
 ********************************************************************************************************************************/

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

/********************************************************************************************************************************
 * 		hnd_get_rtc: rtc resource get handler
 ********************************************************************************************************************************/

void hnd_get_rtc(coap_context_t  *c, struct coap_resource_t *resource,
        coap_address_t *peer, coap_pdu_t *request, str *token,
        coap_pdu_t *response){

	coap_opt_iterator_t optIter;
	unsigned char buf[5];
  unsigned char str[500];
  int len;
	time_t now;

	response->hdr->code = COAP_RESPONSE_CODE(205);

	coap_add_option(response, COAP_OPTION_CONTENT_TYPE, coap_encode_var_bytes(buf, COAP_MEDIATYPE_TEXT_PLAIN), buf);

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
    snprintf((char*)str, 500 , "{\n");
    len = strlen((char*)str);
    snprintf((char*)&str[len], 500 - len, "\"state\":\"%s\",\n", rtc_get_state_text());
    len = strlen((char*)str);
		len += strftime((char*)&str[len], 500 - len, "\"time\":\"%d.%m.%Y %H:%M:%S\"\n", tmp);
    snprintf((char*)&str[len], 500 - len, "}\n");
    len = strlen((char*)str);
		response->length += snprintf((char*)response->data, response->max_size - response->length, "%s", str);
	}

}

/********************************************************************************************************************************
 *    hnd_get_runmode: runmode resource get handler
 ********************************************************************************************************************************/

void hnd_get_runmode(coap_context_t  *c, struct coap_resource_t *resource,
        coap_address_t *peer, coap_pdu_t *request, str *token,
        coap_pdu_t *response){

  coap_opt_iterator_t optIter;
  unsigned char buf[5];
  time_t now;
  unsigned char payload[MAX_PAYLOAD_SIZE];
  int len = 0;
  struct tm *timer;

  response->hdr->code = COAP_RESPONSE_CODE(205);

  coap_add_option(response, COAP_OPTION_CONTENT_TYPE, coap_encode_var_bytes(buf, COAP_MEDIATYPE_TEXT_PLAIN), buf);

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

    len += sprintf((char*)&payload[len], "{\n");
    if(lielas_getRunmode() == RUNMODE_NORMAL){
      len += sprintf((char*)&payload[len], "  \"runmode\":\"normal\"\n");
    }else if(lielas_getRunmode() == RUNMODE_REGISTER){
      len += snprintf((char*)&payload[len], MAX_PAYLOAD_SIZE, "  \"runmode\":\"register\",\n");
      timer = lielas_getEndRegModeTimer();
      if(timer != NULL){
        len += strftime((char*)&payload[len], (MAX_PAYLOAD_SIZE-len), "  \"end\":\"%d.%m.%Y %H:%M:%S\",\n", timer);
      }
    }
    len += sprintf((char*)&payload[len], "}\n");

    coap_add_data(response, len, payload);
  }

}

/********************************************************************************************************************************
 * 		hnd_get_lbus: lbus resource get handler
 ********************************************************************************************************************************/

void hnd_get_lbus(coap_context_t  *c, struct coap_resource_t *resource,
        coap_address_t *peer, coap_pdu_t *request, str *token,
        coap_pdu_t *response){

	coap_opt_iterator_t optIter;
	unsigned char buf[5];
	unsigned char payload[MAX_PAYLOAD_SIZE];
	int len = 0;
	time_t now;

	response->hdr->code = COAP_RESPONSE_CODE(205);

	coap_add_option(response, COAP_OPTION_CONTENT_TYPE, coap_encode_var_bytes(buf, COAP_MEDIATYPE_APPLICATION_JSON), buf);

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

    Lbuscmd *ptr = lbus_getFirstCmd();
    while(ptr != NULL){
      len += sprintf((char*)&payload[len], "{\n");
      len += sprintf((char*)&payload[len], "  \"id\":\"%li\",\n", ptr->id);
      len += sprintf((char*)&payload[len], "  \"address\":\"%s\",\n", ptr->address);
      len += sprintf((char*)&payload[len], "  \"cmd\":\"%s\",\n", ptr->cmd);
      len += sprintf((char*)&payload[len], "  \"payload\":%s\n", ptr->payload);
      len += sprintf((char*)&payload[len], "  \"handled\":\"%s\"\n", (ptr->handled)?"true":"false");
      len += sprintf((char*)&payload[len], "}\n");
      ptr = lbus_getNextCmd();
    }
		coap_add_data(response, len, payload);
	}

}

/********************************************************************************************************************************
 * 		hnd_put_lbus: lbus resource put handler
 ********************************************************************************************************************************/

void hnd_put_lbus(coap_context_t  *c, struct coap_resource_t *resource,
        coap_address_t *peer, coap_pdu_t *request, str *token,
        coap_pdu_t *response){

	coap_opt_iterator_t optIter;
	unsigned char buf[5];
	time_t now;
	jsmn_parser json;
	jsmnerr_t r;
	jsmntok_t tokens[MAX_JSON_TOKENS];
	int tok;
	int len;
	int parseError = 0;
	unsigned char payload[MAX_PAYLOAD_SIZE];
  unsigned char *requestData;
  size_t requestLen;
  Lbuscmd *cmd;
  time_t rawtime;
  struct tm *tmnow;
  
	response->hdr->code = COAP_RESPONSE_CODE(205);

	coap_add_option(response, COAP_OPTION_CONTENT_TYPE, coap_encode_var_bytes(buf, COAP_MEDIATYPE_APPLICATION_JSON), buf);

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
		//parse cmd
    
    
    if(!coap_get_data(request, &requestLen, &requestData)){
			lielas_log((unsigned char*)"failed to get request data", LOG_LEVEL_WARN);
			response->hdr->code = COAP_RESPONSE_CODE(500);
			return;
    }
    
    if(requestData == NULL){
			lielas_log((unsigned char*)"failed to get request data", LOG_LEVEL_WARN);
			response->hdr->code = COAP_RESPONSE_CODE(500);
			return;
    }
    
    requestData[requestLen] = 0;
    
		jsmn_init(&json);
		r = jsmn_parse(&json, (char*)request->data, tokens, MAX_JSON_TOKENS);

		if(r != JSMN_SUCCESS){
			lielas_log((unsigned char*)"JSON parse error parsing put lbus cmd:", LOG_LEVEL_WARN);
			response->hdr->code = COAP_RESPONSE_CODE(400);
      exit(0);
			return;
		}

		cmd = lbus_createCmd(0);
		if(cmd == NULL) {
			lielas_log((unsigned char*)"Couldn't create lbus cmd", LOG_LEVEL_WARN);
			response->hdr->code = COAP_RESPONSE_CODE(500);
			return;
      
		}

		for(tok = 0; tok< json.toknext && !parseError; tok++){
      // parse address
			if(tokens[tok].type == JSMN_STRING){
				if(strncmp((char*)&request->data[tokens[tok].start], LBUS_TOK_ADDRESS, strlen(LBUS_TOK_ADDRESS)) == 0){
					tok += 1;
					len = tokens[tok].end - tokens[tok].start;
					if(len > LBUF_MAX_ADDRESS_SIZE){
						parseError = 1;
						break;
					}
					memcpy(cmd->address, (char*)&request->data[tokens[tok].start], len);
					cmd->address[len] = 0;
	      // parse user
				}else if(strncmp((char*)&request->data[tokens[tok].start], LBUS_TOK_USER, strlen(LBUS_TOK_CMD)) == 0){
          tok += 1;
          len = tokens[tok].end - tokens[tok].start;
          if(len > LBUF_MAX_USER_SIZE){
            parseError = 1;
            break;
          }
          memcpy(cmd->user, (char*)&request->data[tokens[tok].start], len);
          cmd->user[len] = 0;
        // parse cmd
        }else if(strncmp((char*)&request->data[tokens[tok].start], LBUS_TOK_CMD, strlen(LBUS_TOK_CMD)) == 0){
					tok += 1;
					len = tokens[tok].end - tokens[tok].start;
					if(len > LBUF_MAX_CMD_SIZE){
						parseError = 1;
						break;
					}
					memcpy(cmd->cmd, (char*)&request->data[tokens[tok].start], len);
					cmd->cmd[len] = 0;
				// parse payload
				}else if(strncmp((char*)&request->data[tokens[tok].start], LBUS_TOK_PAYLOAD, strlen(LBUS_TOK_PAYLOAD)) == 0){
					tok += 1;

					len = tokens[tok].end - tokens[tok].start;
					if(len > LBUF_MAX_PAYLOAD_SIZE){
						parseError = 1;
						break;
					}
					memcpy(cmd->payload, &request->data[tokens[tok].start], len);
					cmd->payload[len] = 0;
				}
			}
		}

		if(parseError){
			lielas_log((unsigned char*)"JSON parse error parsing put lbus cmd", LOG_LEVEL_WARN);
			response->hdr->code = COAP_RESPONSE_CODE(400);
			return;
		}
    
    if(cmd->cmd[0] == 0){
			lielas_log((unsigned char*)"Error parsing lbus cmd, no command given", LOG_LEVEL_WARN);
			response->hdr->code = COAP_RESPONSE_CODE(400);
			return;
    }
    
    if(cmd->address[0] == 0){
			lielas_log((unsigned char*)"Error parsing lbus cmd, no address given", LOG_LEVEL_WARN);
			response->hdr->code = COAP_RESPONSE_CODE(400);
			return;
    }
    
    if(cmd->user[0] == 0){
			lielas_log((unsigned char*)"Error parsing lbus cmd, no user given", LOG_LEVEL_WARN);
			response->hdr->code = COAP_RESPONSE_CODE(400);
			return;
    }


	  //create timestamp
	  time(&rawtime);
	  tmnow = gmtime(&rawtime);
	  cmd->tmrecv = *tmnow;

    lbus_add(cmd, 1);
    free(cmd);



	  Lbuscmd *ptr = lbus_getFirstCmd();
	  len = 0;
	  while(ptr != NULL){
	    len += lbus_printCmd((char*)&payload[len], MAX_PAYLOAD_SIZE, ptr);
	    ptr = lbus_getNextCmd();
	  }
		coap_add_data(response, len, payload);
	}

}

/********************************************************************************************************************************
 * 		void init_resources(coap_context_t *c): register coap resources
 ********************************************************************************************************************************/

void init_resources(coap_context_t *c){
	coap_resource_t *r;

	r = coap_resource_init(NULL, 0, 0);
	coap_register_handler(r, COAP_REQUEST_GET, hnd_get_index);
	coap_add_attr(r, (unsigned char *) "ct", 2, (unsigned char*) "0", 1, 0);
	coap_add_attr(r, (unsigned char *) "title", 5, (unsigned char *) "\"Lielas\"", 14, 0);
	coap_add_resource(c, r);

	r = coap_resource_init((unsigned char *) "rtc", 3, 0);
	coap_register_handler(r, COAP_REQUEST_GET, hnd_get_rtc);
	coap_add_attr(r, (unsigned char *)"ct", 2, (unsigned char*)"0", 1, 0);
	coap_add_attr(r, (unsigned char *)"title", 2, (unsigned char*)"\"real time clock\"", 16, 0);
	coap_add_attr(r, (unsigned char *)"rt", 2, (unsigned char*)"\"text\"", 7, 0);
	coap_add_resource(c, r);
	
  r = coap_resource_init((unsigned char *) "runmode", 7, 0);
  coap_register_handler(r, COAP_REQUEST_GET, hnd_get_runmode);
  coap_add_attr(r, (unsigned char *)"ct", 2, (unsigned char*)"0", 1, 0);
  coap_add_attr(r, (unsigned char *)"title", 2, (unsigned char*)"\"runmode\"", 16, 0);
  coap_add_attr(r, (unsigned char *)"rt", 2, (unsigned char*)"\"application/json\"", 7, 0);
  coap_add_resource(c, r);

	r = coap_resource_init((unsigned char *) "lbus", 4, 0);
	coap_register_handler(r, COAP_REQUEST_GET, hnd_get_lbus);
	coap_register_handler(r, COAP_REQUEST_PUT, hnd_put_lbus);
	coap_add_attr(r, (unsigned char *)"ct", 2, (unsigned char*)"0", 1, 0);
	coap_add_attr(r, (unsigned char *)"title", 2, (unsigned char*)"\"lbus\"", 16, 0);
	coap_add_attr(r, (unsigned char *)"rt", 2, (unsigned char*)"\"application/json\"", 7, 0);
	coap_add_resource(c, r);


	my_clock_base = clock_offset;
}

/********************************************************************************************************************************
 * 		coap_context_t *get_context()
 ********************************************************************************************************************************/

coap_context_t *get_context(){
	coap_context_t *c = NULL;
	int s;
	struct addrinfo hints;
	struct addrinfo *result, *rp;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE | AI_NUMERICHOST;

	s = getaddrinfo("::", "5684", &hints, &result);
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

/********************************************************************************************************************************
 * 		void COAPfreeContext()
 ********************************************************************************************************************************/

void COAPfreeContext(){
	coap_free_context( ctx );
}

/********************************************************************************************************************************
 * 		int COAPinit()
 ********************************************************************************************************************************/

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

/********************************************************************************************************************************
 * 		void COAPhandleServer()
 ********************************************************************************************************************************/

void *COAPhandleServer(){
	fd_set readfds;
	coap_queue_t *nextpdu;
	coap_tick_t now;
	struct timeval tv = {1, 0};
	struct timeval *timeout;
	int result;

	while(1){

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

  return NULL;
}

coap_tid_t COAPgetID(){
	if(coapID > 30000)
		coapID = 1;
	return coapID++;
}











