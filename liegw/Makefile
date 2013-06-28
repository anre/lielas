include Makefile.inc

BIN=liegw
DESTDIR=
BINDIR=/usr/bin/

prefix = /usr/local
# Prefix for each installed program, 
# normally empty or `g'.
binprefix = 

# The directory to install tar in.
bindir = $(prefix)/bin

# The directory to install the info files in.
infodir = $(prefix)/info

LIBS := -lpq -lcoap -lpthread
C_SRCS :=
O_SRCS :=
C_DEPS :=

SUBDIRS := \
src \
src/coap \
src/lielas \
src/sql \
src/jsmn \
src/rtc

all: $(BIN)

include src/Makefile.sub
include src/coap/Makefile.sub
include src/lielas/Makefile.sub
include src/sql/Makefile.sub
include src/jsmn/Makefile.sub
include src/rtc/Makefile.sub

$(BIN): $(OBJS) 
	@echo 'Building target: $@'
	$(CC) $(CFLAGS) -o $(BIN) $(OBJS) $(LFLAGS) $(LIBS) 
	@echo 'Finished building target: $@'
	@echo ' '

install:
	mkdir -p $(DESTDIR)$(BINDIR) 
	cp $(BIN) $(DESTDIR)$(BINDIR)
	chmod +x $(DESTDIR)$(BINDIR)$(BIN)

uninstall:
	@echo 'deleting $(DISTDIR)$(BINDIR)$(BIN)'
	rm -f $(DISTDIR)$(BINDIR)$(BIN)

clean:
	rm -rf $(OBJS)
	rm -f $(BIN)

