CC = gcc
CFLAGS = -I/usr/include/postgresql -fsigned-char -O0 -ggdb -Wall -Werror -Wfatal-errors
LD = gcc
LDFLAGS = -L/usr/lib -L/usr/include/postgresql/ -L/usr/lib/arm-linux-gnueabihf
AR = ar
ARFLAGS = 
RANLIB = ranlib
RM = rm
ECHO = echo

SHELL = /bin/sh

.SILENT:

