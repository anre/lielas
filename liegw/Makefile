include Makefile.inc

BIN=liegw
DESTDIR=lielas/
BINDIR=bin/

prefix = /usr/local/

LIBS := -lm -lpq -lcoap -lpthread
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
	@echo 'installing to $(prefix)$(DESTDIR)$(BINDIR)'
	mkdir -p $(prefix)$(DESTDIR)$(BINDIR) 
	cp $(BIN) $(prefix)$(DESTDIR)$(BINDIR)
	chmod +x $(prefix)$(DESTDIR)$(BINDIR)$(BIN)

uninstall:
	@echo 'deleting $(prefix)$(DESTDIR)$(BINDIR)$(BIN)'
	rm -f $(prefix)$(DESTDIR)$(BINDIR)$(BIN)

clean:
	rm -rf $(OBJS)
	rm -f $(BIN)

