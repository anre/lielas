include Makefile.inc

BIN=ipchanger
DESTDIR=
BINDIR=/usr/sbin/

prefix=/usr/local/lielas

SUBDIRS:=src

all: $(BIN)

include src/Makefile.sub

$(BIN): $(OBJS)
	@echo 'LD: '
	$(CC) $(CFLAGS) -o $(BIN) $(OBJS) $(LFLAGS) $(LIBS) 

install:
	@echo 'installing to /usr/local/lielas/bin'
	mkdir -p /usr/local/lielas/bin
	cp $(BIN) /usr/local/lielas/bin
	chown root.root /usr/local/lielas/bin/ipchanger

clean:
	rm -rf $(OBJS)
	rm -f $(BIN)

  
