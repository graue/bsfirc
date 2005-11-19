</$objtype/mkfile

CC=pcc
CFLAGS=-I. -B -c -DPLAN9

LIB=libirc.a$O

OFILES= \
	commands.$O\
	irclib.$O\
	message.$O\
	misc.$O\
	packet.$O\
	socket.$O\
	split.$O\

</sys/src/cmd/mklib

install:V:	$LIB
	date

$LIB:	$LIBOBJ
	ar vu $LIB $OFILES
	
clean:
	rm -f *.[$OS] *.a[$OS]
