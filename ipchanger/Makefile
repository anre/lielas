include Makefile.inc

BIN=ipchanger
DESTDIR=
BINDIR=/usr/sbin/

prefix=/usr/local/lielas

SUBDIRS:=src

all: $(BIN)

include src/Makefile.sub

$(BIN): $(OBJS)
	@echo 'Building target: $@'
	$(CC) $(CFLAGS) -o $(BIN) $(OBJS) $(LFLAGS) $(LIBS) 
	@echo 'Finished building target: $@'
	@echo ' '

clean:
	rm -rf $(OBJS)
	rm -f $(BIN)

  