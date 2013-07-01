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

#ifndef MODUL_H
#define MODUL_H

#include "lielas.h"
#include "channel.h"

typedef struct Lmodul_struct{
  unsigned int id;
  char address[IPV6_STR_LEN];
  int channels;
  char mint[INT_STR_LEN];
  char pint[INT_STR_LEN];
  char aint[INT_STR_LEN];
  Lchannel *channel[MAX_CHANNELS];
}Lmodul;


Lmodul *LcreateModul();
void LdeleteModul(Lmodul *m);
void LaddChannel(Lmodul *m, Lchannel *c);
void LremoveChannel(Lmodul *m, Lchannel *c);

#endif /* MODUL_H */
