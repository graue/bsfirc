</$objtype/mkfile
BIN=/$objtype/bin

TARG=bsflite
CC=pcc
CFLAGS=-I. -B -c -DPLAN9

OFILES= \
	bsfirc.$O\
	cleaner.$O\
	input.$O\
	irc.$O\
	lists.$O\
	log.$O\
	output.$O\
	p9win.$O\
	screen.$O\
	irclib/libirc.a$O\

HFILES= \
	bsfirc.h\

</sys/src/cmd/mkone

irclib/libirc.a$O:
	cd irclib
	mk

clean:
	cd irclib
	mk clean
	cd ..
	rm -f *.[$OS] [$OS].out $TARG
