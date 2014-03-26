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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#include "tcpserver.h"
#include "jsmn/jsmn.h"
#include "lielas/lbus.h"
#include "rtc/rtc.h"

void tcp_log(char* msg);

void* tcpserver(){
  int listenfd = 0;
  int connfd = 0;
  struct sockaddr_in serverAddress;
  char sendBuf[TCP_SEND_BUF_SIZE];
  char readBuf[TCP_SEND_BUF_SIZE];
  int bytes;
  jsmn_parser json;
  jsmnerr_t jsonErr;
  jsmntok_t tokens[MAX_JSON_TOKENS];
  int tok;
  int parseError = 0;
  Lbuscmd *cmd;
  int len;
  time_t rawtime;
  struct tm *tmnow;
  
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  memset(&serverAddress, '0', sizeof(serverAddress));
  memset(sendBuf, '0', sizeof(sendBuf));
  
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddress.sin_port = htons(5684);
  
  bind(listenfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
  
  listen(listenfd, 2);
  
  while(1){
    connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
    if(connfd > 0){
      bytes = recv(connfd, readBuf, TCP_SEND_BUF_SIZE, 0);
      if(bytes > 0){
        readBuf[bytes] = 0;
        
        //parse data
        jsmn_init(&json);
        jsonErr = jsmn_parse(&json, readBuf, tokens, MAX_JSON_TOKENS);
        
        if(jsonErr != JSMN_SUCCESS){
          tcp_log("error parsing data");
          snprintf(sendBuf, TCP_SEND_BUF_SIZE, "error\nparsing data\n");
          write(connfd, sendBuf, strlen(sendBuf));
        }else{
          cmd = lbus_createCmd(0);
          if(cmd == NULL){
            tcp_log("error creating lbus cmd");
            snprintf(sendBuf, TCP_SEND_BUF_SIZE, "error\ncreating lbuscmd\n");
            write(connfd, sendBuf, strlen(sendBuf));
          }else{
            for(tok = 0; tok < json.toknext && !parseError; tok++){
              if(tokens[tok].type == JSMN_STRING){
                //parse address
                if(strncmp(&readBuf[tokens[tok].start], LBUS_TOK_ADDRESS, strlen(LBUS_TOK_ADDRESS)) == 0){
                  tok += 1;
                  len = tokens[tok].end - tokens[tok].start;
                  if(len >= LBUF_MAX_ADDRESS_SIZE){
                    parseError = 1;
                  }else{
                    memcpy(cmd->address, &readBuf[tokens[tok].start], len);
                    cmd->address[len]  = 0;
                  }
                }
                //parse user
                if(strncmp(&readBuf[tokens[tok].start], LBUS_TOK_USER, strlen(LBUS_TOK_USER)) == 0){
                  tok += 1;
                  len = tokens[tok].end - tokens[tok].start;
                  if(len >= LBUF_MAX_USER_SIZE){
                    parseError = 1;
                  }else{
                    memcpy(cmd->user, &readBuf[tokens[tok].start], len);
                    cmd->user[len]  = 0;
                  }
                }
                //parse cmd
                if(strncmp(&readBuf[tokens[tok].start], LBUS_TOK_CMD, strlen(LBUS_TOK_CMD)) == 0){
                  tok += 1;
                  len = tokens[tok].end - tokens[tok].start;
                  if(len >= LBUF_MAX_CMD_SIZE){
                    parseError = 1;
                  }else{
                    memcpy(cmd->cmd, &readBuf[tokens[tok].start], len);
                    cmd->cmd[len]  = 0;
                  }
                }
                //parse payload
                if(strncmp(&readBuf[tokens[tok].start], LBUS_TOK_PAYLOAD, strlen(LBUS_TOK_PAYLOAD)) == 0){
                  tok += 1;
                  len = tokens[tok].end - tokens[tok].start;
                  if(len >= LBUF_MAX_PAYLOAD_SIZE){
                    parseError = 1;
                  }else{
                    memcpy(cmd->payload, &readBuf[tokens[tok].start], len);
                    cmd->payload[len]  = 0;
                  }
                }
              }
            }
            if(parseError){
              tcp_log("error parsing lbus cmd");
              snprintf(sendBuf, TCP_SEND_BUF_SIZE, "error\nparsing lbus cmd\n");
              write(connfd, sendBuf, strlen(sendBuf));
            }else{
              if(cmd->cmd[0] == 0){
                tcp_log("error parsing lbus cmd: no command given");
                snprintf(sendBuf, TCP_SEND_BUF_SIZE, "error\nparsing lbus cmd: no command given\n");
                write(connfd, sendBuf, strlen(sendBuf));
              }else if(cmd->address[0] == 0){
                tcp_log("error parsing lbus cmd: no address given");
                snprintf(sendBuf, TCP_SEND_BUF_SIZE, "error\nparsing lbus cmd: no address given\n");
                write(connfd, sendBuf, strlen(sendBuf));
              }else if(cmd->user[0] == 0){
                tcp_log("error parsing lbus cmd: no user given");
                snprintf(sendBuf, TCP_SEND_BUF_SIZE, "error\nparsing lbus cmd: no user given\n");
                write(connfd, sendBuf, strlen(sendBuf));
              }else{	  //create timestamp
                time(&rawtime);
                tmnow = gmtime(&rawtime);
                cmd->tmrecv = *tmnow;
                if(!strcmp(cmd->cmd, TCP_CMD_RTC_STATE)){
                  //answer rtc state immediately
                  snprintf(sendBuf, TCP_SEND_BUF_SIZE, "ok\n%s\n", rtc_get_state_text());
                  write(connfd, sendBuf, strlen(sendBuf));
                }else if(!strcmp(cmd->cmd, TCP_CMD_TRC_TIME)){
                  //answer rtc time immediately
                  strftime(sendBuf, TCP_SEND_BUF_SIZE, "ok\n%R %d.%m.%Y\n", tmnow);
                  write(connfd, sendBuf, strlen(sendBuf));
                }else{
                  if(lbus_add(cmd, 1)){
                    tcp_log("error error saving lbus cmd");
                    snprintf(sendBuf, TCP_SEND_BUF_SIZE, "error\nsaving lbus cmd\n");
                    write(connfd, sendBuf, strlen(sendBuf));
                  }else{
                    snprintf(sendBuf, TCP_SEND_BUF_SIZE, "ok\n");
                    write(connfd, sendBuf, strlen(sendBuf));
                  }
                }
                free(cmd);
              }
            }
          }
        }
      }
      close(connfd);
    }
  }
  return NULL;
}

void tcp_log(char* msg){
	char timestamp[50];
	time_t rawtime;
	struct tm *now;
  
  	//create timestamp
	time(&rawtime);
	now = gmtime(&rawtime);
	strftime(timestamp, 50, "[%d.%m.%Y %H:%M:%S]", now);
  
	FILE *f = fopen("/usr/local/lielas/logs/tcpserver.log", "a");
	if(f != NULL){
		fprintf(f, "%s%s\n", timestamp, msg);
		fclose(f);
	}else{
		printf("error: failed to open tcp logfile\n");
	}

  
}






