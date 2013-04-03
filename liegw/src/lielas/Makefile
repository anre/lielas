include ../../Makefile.inc

CFLAGS = $(PRJCFLAGS) 
OBJLIBS = ../../liewebgw.a 
OBJS = channel.o device.o devicecontainer.o deviceevent.o modul.o

all : $(OBJLIBS)

../../liewebgw.a : force_look
	$(ECHO) $(AR) $(ARFLAGS) rv ../../liewebgw.a $?
	$(AR) $(ARFLAGS) rv ../../liewebgw.a $?
	$(ECHO) $(RANLIB) ../../liewebgw.a
	$(RANLIB) ../../liewebgw.a 

clean :
	$(ECHO) cleaning up in subdir
	-$(RM) -f $(OBJS)
	cd subsubdir; $(MAKE) $(MFLAGS) clean

force_look :
	true
