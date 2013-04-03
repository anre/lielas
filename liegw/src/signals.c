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

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "signals.h"
#include "log.h"

static int sigint;
static int sigusr1;

static void check_sig();

/********************************************************************************************************************************
 * 		int sig_init(): initialize signal handlers
 ********************************************************************************************************************************/
int sig_init(){
	struct sigaction sigintact;
	struct sigaction sigusr1act;

	sigintact.sa_handler = &check_sig;
	sigemptyset(&sigintact.sa_mask);
	sigintact.sa_flags = 0;

	sigusr1act.sa_handler = &check_sig;
	sigemptyset(&sigusr1act.sa_mask);
	sigusr1act.sa_flags = 0;

	if(sigaction(SIGINT, &sigintact, NULL) == -1){
		return -1;
	}

	if(sigaction(SIGUSR1, &sigusr1act, NULL) == -1){
		return -1;
	}

	return 0;
}

/********************************************************************************************************************************
 * 		int sig_sigint_received(): returns 1 if sigint occured, else 0
 ********************************************************************************************************************************/
int sig_sigint_received(){
	if(sigint ==1)
		return 1;
	return 0;
}

/********************************************************************************************************************************
 * 		int sig_sigusr1_received(): returns 1 if sigusr1 occured, else 0
 ********************************************************************************************************************************/
int sig_sigusr1_received(){
	if(sigusr1 ==1)
		return 1;
	return 0;
}

/********************************************************************************************************************************
 * 		void sig_reset_sigusr1(): reset sigusr1 to 0
 ********************************************************************************************************************************/
void sig_reset_sigusr1(){
	sigusr1 = 0;
}

/********************************************************************************************************************************
 * 		static void check_sig(int signr): checks signal and sets related flag
 ********************************************************************************************************************************/
static void check_sig(int signr){
	switch(signr){
	case SIGINT:
		sigint = 1;
		break;
	case SIGUSR1:
		sigusr1 = 1;
		break;
	}
}












