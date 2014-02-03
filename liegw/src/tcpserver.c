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
#incldue <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#include "tcpserver.h"

void* tcpserver(){
  int listenfd = 0
  int connfd = 0;
  struct sockaddr_in serverAddress;
  char sendBuf[TCP_SEND_BUF_SIZE];
  
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  memset(&serverAddress, '0', sizeof(serverAddress));
  memset(sendBuf, '0', sizeof(sendBuf));
  
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddress.sin_port = htons(5684);
  
  bind(listenfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
  
  listen(listenfd, 2);
  
  while(1){
    connfd = accept(listenfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    if(connfd > 0){
      snprintf(sendBuf, TCP_SEND_BUF_SIZE, "hell yeah :)");
      write(connfd, sendBuf, strlen(sendBuf));
      close(connfd);
    }
  }
  return NULL;
}






