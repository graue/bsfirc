EXEC=irctest
#AR=i386-pc-msdosdjgpp-ar
#AR=i386-mingw32msvc-ar
AR=ar
#RANLIB=i386-pc-msdosdjgpp-ranlib
#RANLIB=i386-mingw32msvc-ranlib
RANLIB=ranlib
LIB=libirc.a
#CC=i386-pc-msdosdjgpp-gcc
#CC=i386-mingw32msvc-gcc
CC=gcc
CFLAGS=-O2 -g3 -Wall -pedantic -pipe
#CFLAGS+=-I/usr/local/djgpp/watt/inc
#
# Add -lsocket -lnsl for Solaris
#
LIBS=-lirc
#LIBS+=-lwatt -L/usr/local/djgpp/watt/lib
LDFLAGS=-L.
OBJECTS=socket.o packet.o irclib.o misc.o message.o commands.o split.o alloc.o

$(EXEC):$(OBJECTS) test.o
	$(AR) rcv $(LIB) $(OBJECTS)
	$(RANLIB) $(LIB)
#	$(CC) $(CFLAGS) $(LDFLAGS) -o $(EXEC) test.o $(LIBS)

clean:
	rm -f *.o *~ $(EXEC) $(LIB)

.c.pro:                        
	$(SED) -n -f $(srcdir)/makeprotos-sed $< > $@
