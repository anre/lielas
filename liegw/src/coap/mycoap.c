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

#include "mycoap.h"
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#define BUFSIZE  40

int flags = 0;

#define FLAGS_BLOCK 0x01

unsigned char method = MYCOAP_METHOD_GET; /* the method we are using in our requests */

static coap_list_t *optlist = NULL;

unsigned int wait_seconds = 2; /* default timeout in seconds */
coap_tick_t max_wait;		/* global timeout (changed by set_timeout()) */

/* reading is done when this flag is set */
static int ready = 0;

static str payload = { 0, NULL }; /* optional payload to send */

unsigned char msgtype = COAP_MESSAGE_CON; /* usually, requests are sent confirmable */

coap_block_t block = { .num = 0, .m = 0, .szx = 6 };

static str output_file = { 0, NULL }; /* output file name */
static FILE *file = NULL;	/* output file stream */

static coap_buf *mycoapbuf;

static inline void
set_timeout(coap_tick_t *timer, const unsigned int seconds) {
  coap_ticks(timer);
  *timer += seconds * COAP_TICKS_PER_SECOND;
}

int check_token(coap_pdu_t *received) {
  coap_opt_iterator_t opt_iter;
  coap_list_t *option;
  str token1 = { 0, NULL }, token2 = { 0, NULL };

  if (coap_check_option(received, COAP_OPTION_TOKEN, &opt_iter)) {
    token1.s = COAP_OPT_VALUE(opt_iter.option);
    token1.length = COAP_OPT_LENGTH(opt_iter.option);
  }

  for (option = optlist; option; option = option->next) {
    if (COAP_OPTION_KEY(*(coap_option *)option->data) == COAP_OPTION_TOKEN) {
      token2.s = COAP_OPTION_DATA(*(coap_option *)option->data);
      token2.length = COAP_OPTION_LENGTH(*(coap_option *)option->data);
      break;
    }
  }

  return token1.length == token2.length &&
    memcmp(token1.s, token2.s, token1.length) == 0;
}

coap_context_t *
get_ccontext(const char *node, const char *port) {
  coap_context_t *ctx = NULL;
  int s;
  struct addrinfo hints;
  struct addrinfo *result, *rp;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
  hints.ai_socktype = SOCK_DGRAM; /* Coap uses UDP */
  hints.ai_flags = AI_PASSIVE | AI_NUMERICHOST | AI_NUMERICSERV | AI_ALL;

  s = getaddrinfo(node, port, &hints, &result);
  if ( s != 0 ) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
    return NULL;
  }

  /* iterate through results until success */
  for (rp = result; rp != NULL; rp = rp->ai_next) {
    coap_address_t addr;

    if (rp->ai_addrlen <= sizeof(addr.addr)) {
      coap_address_init(&addr);
      addr.size = rp->ai_addrlen;
      memcpy(&addr.addr, rp->ai_addr, rp->ai_addrlen);

      ctx = coap_new_context(&addr);
      if (ctx) {
	/* TODO: output address:port for successful binding */
	goto finish;
      }
    }
  }

  fprintf(stderr, "no context available for interface '%s'\n", node);

 finish:
  freeaddrinfo(result);
  return ctx;
}

int
resolve_address(const str *server, struct sockaddr *dst) {

  struct addrinfo *res, *ainfo;
  struct addrinfo hints;
  static char addrstr[256];
  int error, len=-1;

  memset(addrstr, 0, sizeof(addrstr));
  if (server->length)
    memcpy(addrstr, server->s, server->length);
  else
    memcpy(addrstr, "localhost", 9);

  memset ((char *)&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_family = AF_UNSPEC;

  error = getaddrinfo(addrstr, "", &hints, &res);

  if (error != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
    return error;
  }

  for (ainfo = res; ainfo != NULL; ainfo = ainfo->ai_next) {
    switch (ainfo->ai_family) {
    case AF_INET6:
    case AF_INET:
      len = ainfo->ai_addrlen;
      memcpy(dst, ainfo->ai_addr, len);
      goto finish;
    default:
      break;
    }
  }

 finish:
  freeaddrinfo(res);
  return len;
}

coap_list_t *
new_option_node(unsigned short key, unsigned int length, unsigned char *data) {
  coap_option *option;
  coap_list_t *node;

  option = coap_malloc(sizeof(coap_option) + length);
  if ( !option )
    goto error;

  COAP_OPTION_KEY(*option) = key;
  COAP_OPTION_LENGTH(*option) = length;
  memcpy(COAP_OPTION_DATA(*option), data, length);

  /* we can pass NULL here as delete function since option is released automatically  */
  node = coap_new_listnode(option, NULL);

  if ( node )
    return node;

 error:
  perror("new_option_node: malloc");
  coap_free( option );
  return NULL;
}

int
append_to_output(const unsigned char *data, size_t len) {
  size_t written;

  if (!file) {
    if (!output_file.s || (output_file.length && output_file.s[0] == '-'))
      file = stdout;
    else {
      if (!(file = fopen((char *)output_file.s, "w"))) {
	perror("fopen");
	return -1;
      }
    }
  }

  do {
    written = fwrite(data, 1, len, file);
    len -= written;
    data += written;
  } while ( written && len );
  fflush(file);

  return 0;
}

coap_pdu_t *
coap_new_request(coap_context_t *ctx, unsigned char m, coap_list_t *options ) {
  coap_pdu_t *pdu;
  coap_list_t *opt;

  if ( ! ( pdu = coap_new_pdu() ) )
    return NULL;

  pdu->hdr->type = msgtype;
  pdu->hdr->id = coap_new_message_id(ctx);
  pdu->hdr->code = m;

  for (opt = options; opt; opt = opt->next) {
    coap_add_option(pdu, COAP_OPTION_KEY(*(coap_option *)opt->data),
		    COAP_OPTION_LENGTH(*(coap_option *)opt->data),
		    COAP_OPTION_DATA(*(coap_option *)opt->data));
  }

  if (payload.length) {
    if ((flags & FLAGS_BLOCK) == 0)
      coap_add_data(pdu, payload.length, payload.s);
    else
      coap_add_block(pdu, payload.length, payload.s, block.num, block.szx);
  }

  return pdu;
}

static inline coap_opt_t *
get_block(coap_pdu_t *pdu, coap_opt_iterator_t *opt_iter) {
  coap_opt_filter_t f;

  assert(pdu);

  memset(f, 0, sizeof(coap_opt_filter_t));
  coap_option_setb(f, COAP_OPTION_BLOCK1);
  coap_option_setb(f, COAP_OPTION_BLOCK2);

  coap_option_iterator_init(pdu, opt_iter, f);
  return coap_option_next(opt_iter);
}

int order_opts(void *a, void *b) {
  if (!a || !b)
    return a < b ? -1 : 1;

  if (COAP_OPTION_KEY(*(coap_option *)a) < COAP_OPTION_KEY(*(coap_option *)b))
    return -1;

  return COAP_OPTION_KEY(*(coap_option *)a) == COAP_OPTION_KEY(*(coap_option *)b);
}

void message_handler(struct coap_context_t  *ctx,
					 const coap_address_t *remote,
					 coap_pdu_t *sent,
					 coap_pdu_t *received,
					 const coap_tid_t id) {

	  coap_pdu_t *pdu = NULL;
	  coap_opt_t *block_opt;
	  coap_opt_iterator_t opt_iter;
	  unsigned char buf[4];
	  coap_list_t *option;
	  size_t len;
	  unsigned char *databuf;
	  coap_tid_t tid;
	  int i;

	#ifndef NDEBUG
	  if (LOG_DEBUG <= coap_get_log_level()) {
	    debug("** process incoming %d.%02d response:\n",
		  (received->hdr->code >> 5), received->hdr->code & 0x1F);
	    coap_show_pdu(received);
	  }
	#endif

	  /* check if this is a response to our original request */
	  if (!check_token(received)) {
	    /* drop if this was just some message, or send RST in case of notification */
	    if (!sent && (received->hdr->type == COAP_MESSAGE_CON ||
			  received->hdr->type == COAP_MESSAGE_NON))
	      coap_send_rst(ctx, remote, received);
	    return;
	  }

	  switch (received->hdr->type) {
	  case COAP_MESSAGE_CON:
	    /* acknowledge received response if confirmable (TODO: check Token) */
	    coap_send_ack(ctx, remote, received);
	    break;
	  case COAP_MESSAGE_RST:
	    info("got RST\n");
	    return;
	  default:
	    break;
	  }

	  /* output the received data, if any */
	  if (received->hdr->code == COAP_RESPONSE_CODE(205)) {


	    /* Got some data, check if block option is set. Behavior is undefined if
	     * both, Block1 and Block2 are present. */
	    block_opt = get_block(received, &opt_iter);
	    if (!block_opt) {
	      /* There is no block option set, just read the data and we are done. */
	      if (coap_get_data(received, &len, &databuf)){
	    	  //append_to_output(databuf, len);
	    	  if( mycoapbuf->buf != 0){
	    		  mycoapbuf->status = received->hdr->code;
				  for( i = 0; i < len; i++){
					  mycoapbuf->buf[mycoapbuf->len++] = databuf[i];
				  }
				  mycoapbuf->buf[mycoapbuf->len +1] = 0;
	    	  }
	      }

	    } else {
	    	unsigned short blktype = opt_iter.type;

	    	/* TODO: check if we are looking at the correct block number */
	    	if (coap_get_data(received, &len, &databuf)){
		    	  if( mycoapbuf->buf != 0){
		    		  if(mycoapbuf->len == 0){
			    		  mycoapbuf->status = received->hdr->code;
		    		  }
					  for( i = 0; i < len; i++){
						  mycoapbuf->buf[mycoapbuf->len++] = databuf[i];
					  }
					  mycoapbuf->buf[mycoapbuf->len +1] = 0;
		    	  }
	    	}

	    	if (COAP_OPT_BLOCK_MORE(block_opt)) {
	    		/* more bit is set */
	    		debug("found the M bit, block size is %u, block nr. %u\n",
	    			COAP_OPT_BLOCK_SZX(block_opt), COAP_OPT_BLOCK_NUM(block_opt));

	    		/* create pdu with request for next block */
	    		pdu = coap_new_request(ctx, method, NULL); /* first, create bare PDU w/o any option  */
	    		if ( pdu ) {
	    			/* add URI components from optlist */
	    			for (option = optlist; option; option = option->next ) {
	    				switch (COAP_OPTION_KEY(*(coap_option *)option->data)) {
	    				case COAP_OPTION_URI_HOST :
	    				case COAP_OPTION_URI_PORT :
	    				case COAP_OPTION_URI_PATH :
	    				case COAP_OPTION_TOKEN :
	    				case COAP_OPTION_URI_QUERY :
	    					coap_add_option ( pdu, COAP_OPTION_KEY(*(coap_option *)option->data),
	    							COAP_OPTION_LENGTH(*(coap_option *)option->data),
	    							COAP_OPTION_DATA(*(coap_option *)option->data) );
	    					break;
	    				}
	    			}

	    			/* finally add updated block option from response, clear M bit */
	    			/* blocknr = (blocknr & 0xfffffff7) + 0x10; */
	    			debug("query block %d\n", (COAP_OPT_BLOCK_NUM(block_opt) + 1));
	    			coap_add_option(pdu, blktype, coap_encode_var_bytes(buf,
	    					((COAP_OPT_BLOCK_NUM(block_opt) + 1) << 4) |
	    					COAP_OPT_BLOCK_SZX(block_opt)), buf);

	    			if (received->hdr->type == COAP_MESSAGE_CON)
	    				tid = coap_send_confirmed(ctx, remote, pdu);
	    			else
	    				tid = coap_send(ctx, remote, pdu);

	    			if (tid == COAP_INVALID_TID) {
	    				debug("message_handler: error sending new request");
	    				coap_delete_pdu(pdu);
	    			} else {
	    				set_timeout(&max_wait, wait_seconds);
	    				//if (received->hdr->type != COAP_MESSAGE_CON)
	    				//  coap_delete_pdu(pdu);
	    			}
	    			return;
	    		}
	    	}
	    }
	  } else {			/* no 2.05 */

	    /* check if an error was signaled and output payload if so */
	    if (COAP_RESPONSE_CLASS(received->hdr->code) >= 4) {
	      fprintf(stderr, "%d.%02d", (received->hdr->code >> 5), received->hdr->code & 0x1F);
	      mycoapbuf->status = received->hdr->code;
		  if (coap_get_data(received, &len, &databuf)) {
	    	  fprintf(stderr, " ");
	    	  while(len--)
	    		  fprintf(stderr, "%c", *databuf++);
	      }
	      fprintf(stderr, "\n");
	    }

	  }

	  /* finally send new request, if needed */
	  if (pdu && coap_send(ctx, remote, pdu) == COAP_INVALID_TID) {
	    debug("message_handler: error sending response");
	  }
	  coap_delete_pdu(pdu);

	  /* our job is done, we can exit at any time */
	  ready = coap_check_option(received, COAP_OPTION_SUBSCRIPTION, &opt_iter) == NULL;
}

coap_buf *coap_create_buf(){
	coap_buf *cb = malloc(sizeof(coap_buf));
	if(cb == NULL)
		return NULL;
	cb->len = 0;
	cb->status = 0;
	cb->buf = 0;
	return cb;
}

void coap_delete_buf(coap_buf *cb){
	if(cb->buf != NULL){
		free(cb->buf);
	}
	free(cb);
}

int coap_send_cmd(char* uriStr, coap_buf *cb, unsigned char methode, unsigned char *post){


	unsigned char _buf[BUFSIZE];
	unsigned char *buf = _buf;

	coap_uri_t uri;
	size_t buflen;
	int res;
	int result;
	int len;
	static str server;
	unsigned short port = COAP_DEFAULT_PORT;
	coap_address_t dst;
	void *addrptr = NULL;
	coap_context_t  *ctx = NULL;
	static char addr[INET6_ADDRSTRLEN];
	coap_pdu_t  *pdu;
	coap_tid_t tid = COAP_INVALID_TID;
	fd_set readfds;
	coap_queue_t *nextpdu;
	coap_tick_t now;
	struct timeval tv;
	char port_str[10] = "0";

	ready = 0;
	mycoapbuf = cb;
	method = methode;

	mycoapbuf->status = 0;
	mycoapbuf->len = 0;

	coap_split_uri((unsigned char*)uriStr, strlen( uriStr), &uri);

	if(uri.path.length){
		buflen = BUFSIZE;

		res = coap_split_path(uri.path.s, uri.path.length, buf, &buflen);

		while( res-- ){
			coap_insert(&optlist, new_option_node(COAP_OPTION_URI_PATH,
				      	  	  	  	  	  	  	  COAP_OPT_LENGTH(buf),
				      	  	  	  	  	  	  	  COAP_OPT_VALUE(buf)),
						order_opts);

			buf += COAP_OPT_SIZE(buf);
		}
	}

    if (uri.query.length) {
      buflen = BUFSIZE;
      buf = _buf;
      res = coap_split_query(uri.query.s, uri.query.length, buf, &buflen);

      while (res--) {
    	  coap_insert(&optlist, new_option_node(COAP_OPTION_URI_QUERY,
					      	  	  	  	  	  	COAP_OPT_LENGTH(buf),
					      	  	  	  	  	  	COAP_OPT_VALUE(buf)),
    			  	  order_opts);

    	  buf += COAP_OPT_SIZE(buf);
      }
    }

    if(method == MYCOAP_METHOD_POST){
    	len = check_segment(post, strlen((char*)post));
    	if(len < 0){
    		payload.length = 0;
    	}else{
    		if(payload.s != NULL){
    			free(payload.s);
    		}
    		payload.s = (unsigned char*)coap_malloc(len);
    		if(payload.s){
    			payload.length = len;
    			decode_segment(post, strlen((char*)post), payload.s);
    		}
    	}
    }else{
    	payload.length = 0;
    }

    server = uri.host;
    port = uri.port;

    /* resolve destination address where server should be sent */
    res = resolve_address(&server, &dst.addr.sa);

    if (res < 0) {
      fprintf(stderr, "failed to resolve address\n");
      return 0;
    }

    dst.size = res;
    dst.addr.sin.sin_port = htons(port);

    /* add Uri-Host if server address differs from uri.host */

    switch (dst.addr.sa.sa_family) {
    case AF_INET:
      addrptr = &dst.addr.sin.sin_addr;

      /* create context for IPv4 */
      ctx = get_ccontext("0.0.0.0", port_str);
      break;
    case AF_INET6:
      addrptr = &dst.addr.sin6.sin6_addr;

      /* create context for IPv6 */
      ctx = get_ccontext("::", port_str);
      break;
    }

    if (!ctx) {
      coap_log(LOG_EMERG, "cannot create context\n");
      return 0;
    }

    coap_register_option(ctx, COAP_OPTION_BLOCK2);
    coap_register_response_handler(ctx, message_handler);

    /* construct CoAP message */

     if (addrptr
         && (inet_ntop(dst.addr.sa.sa_family, addrptr, addr, sizeof(addr)) != 0)
         && (strlen(addr) != uri.host.length
   	  || memcmp(addr, uri.host.s, uri.host.length) != 0)) {
         /* add Uri-Host */

       coap_insert(&optlist, new_option_node(COAP_OPTION_URI_HOST,
   					  uri.host.length, uri.host.s),
   		order_opts);
     }

     if (! (pdu = coap_new_request(ctx, method, optlist)))
         exit(-1);

     if (pdu->hdr->type == COAP_MESSAGE_CON){
       tid = coap_send_confirmed(ctx, &dst, pdu);
     } else
       tid = coap_send(ctx, &dst, pdu);

     if (pdu->hdr->type != COAP_MESSAGE_CON || tid == COAP_INVALID_TID)
       coap_delete_pdu(pdu);

     while(!(ready && coap_can_exit(ctx))){
		 FD_ZERO(&readfds);
		 FD_SET( ctx->sockfd, &readfds );

		 nextpdu = coap_peek_next( ctx );

		 coap_ticks(&now);

		 /*while ( nextpdu && nextpdu->t <= now ) {
		   coap_retransmit( ctx, coap_pop_next( ctx ));
		   nextpdu = coap_peek_next( ctx );
		 }*/

		 /* set timeout if there is a pdu to send */
		 if(nextpdu){
			 tv.tv_usec = ((nextpdu->t - now) % COAP_TICKS_PER_SECOND) * 1000000 / COAP_TICKS_PER_SECOND;
			 tv.tv_sec = (nextpdu->t - now) / COAP_TICKS_PER_SECOND;
			tv.tv_usec = 0;
			tv.tv_sec = 2;
		 }else{
			tv.tv_usec = 0;
			tv.tv_sec = 2;
		 }

		 result = select(ctx->sockfd + 1, &readfds, 0, 0, &tv);


		 if ( result < 0 ) {		/* error */
		   perror("select");
		 } else if ( result > 0 ) {	/* read from socket */
			 if ( FD_ISSET( ctx->sockfd, &readfds ) ) {
				 coap_read( ctx );	/* read received data */
				 coap_dispatch( ctx );	/* and dispatch PDUs from receivequeue */
			 }
		 } else { /* timeout */
		   coap_ticks(&now);
		   if (max_wait <= now) {
			  info("timeout\n");
			  break;
		   }
		 }
     }

     coap_delete_list(optlist);
     //free(optlist->data);
     //free(optlist);
     optlist = NULL;
 	mycoapbuf = 0;

     coap_free_context( ctx );
     return 1;
}



