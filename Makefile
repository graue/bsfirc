EXEC=bsfirc
#EXEC=bsfirc.exe
#CC=i386-pc-msdosdjgpp-gcc
#CC=i386-mingw32msvc-gcc
CC=gcc
#
# Remove -DTIMESTAMPS to remove timestamps on messages/events
#
# Add -DBELL_ON_INCOMING to sound an alert when a message
# 	comes in.
#
CFLAGS=-O2 -g3 -pedantic -Wall
CFLAGS+=-DTIMESTAMPS
#CFLAGS+=-DNETSPEAK_CLEANER
CFLAGS+=-DSMARTALLOC
#CFLAGS+=-I/usr/local/djgpp/watt/inc
#
# Add -lsocket -lnsl for Solaris
#
LIBS=-lirc
#LIBS+=-lwatt
#LIBS+=-lwsock32
LDFLAGS=-Lirclib/
#LDFLAGS+=-L/usr/local/djgpp/watt/lib
OBJECTS=bsfirc.o cleaner.o screen.o output.o irc.o input.o log.o lists.o alloc.o smartall.o

all:
	(cd irclib && $(MAKE) $(MFLAGS))
	$(MAKE) $(EXEC)

$(EXEC):$(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(EXEC) $(OBJECTS) $(LIBS)

clean:
	rm -f *.o *~ $(EXEC)

realclean:
	(cd irclib && $(MAKE) clean)
	$(MAKE) clean
