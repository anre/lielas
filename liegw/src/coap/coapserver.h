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


#ifndef COAPSERVER_H_
#define COAPSERVER_H_

#include "libcoap/config.h"
#include "libcoap/resource.h"
#include "libcoap/coap.h"
#include "libcoap/coap_time.h"

int COAPinit();
void *COAPhandleServer();
void COAPfreeContext();
coap_tid_t COAPgetID();

extern coap_tid_t coapID;

#endif /* COAPSERVER_H_ */
