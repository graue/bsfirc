/**  _        _ _ _
 ** (_)_ _ __| (_) |__
 ** | | '_/ _| | | '_ \
 ** |_|_| \__|_|_|_.__/
 **
 ** A simple library for creating IRC clients.
 **
 ** (C) 2005 by Claudio Leite
 **
 ** Please see the COPYING file for more details.
 **/

#ifndef IRCLIB_H
#define IRCLIB_H

#ifdef PLAN9
#define _POSIX_SOURCE
#define _BSD_EXTENSION
#define _POSIX_EXTENSION
#include <errno.h>

#define uint32_t unsigned long
#define uint16_t unsigned short
#define uint8_t unsigned char
#define int32_t long
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#ifdef MACINTOSH_CLASSIC
#include <Types.h>
#include <MacTCP.h>
#include <AddressXlation.h>
#include <time.h>
#else
#ifndef __MINGW32__
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#else
#include <winsock.h>
#include <stdint.h>
#include <time.h>
#endif
#endif

#ifdef MACINTOSH_CLASSIC
typedef void    fd_set;
typedef unsigned long uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
char           *strdup(char *);
char           *strcasecmp(char *, char *);
void            mactcp_close(void *handle);
OSErr           mactcp_recv(void *handle, char *inbuf, size_t len);
#endif

#ifdef linux
#include <time.h>
#endif

#if defined(__APPLE__) || defined(linux)
#include <stdint.h>
#endif

#include <string.h>
#include <ctype.h>

#ifdef __DJGPP__
typedef unsigned long uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
#endif

typedef int     IRCLIB_RET;

#define IRCLIB_RET_ERROR -1
#define IRCLIB_RET_OK 1

#define NUM_CALLBACKS 22
enum {
	IRCLIB_MOTD,
	IRCLIB_READY,
	IRCLIB_JOIN,
	IRCLIB_PART,
	IRCLIB_NOTICE,
	IRCLIB_PRIVMSG,
	IRCLIB_CTCP,
	IRCLIB_MODE,
	IRCLIB_CHANMSG,
	IRCLIB_QUIT,
	IRCLIB_CHANUSER,
	IRCLIB_SERVER_NAME,
	IRCLIB_NAMESDONE,
	IRCLIB_NICKINUSE,
	IRCLIB_TOPIC,
	IRCLIB_WHOIS_USERHOST,
	IRCLIB_WHOIS_SERVER,
	IRCLIB_WHOIS_CHANNELS,
	IRCLIB_NOTICE_AUTH,
	IRCLIB_NICKCHANGE,
	IRCLIB_ERROR
};

enum {
	C_MODE_OP,
	C_MODE_BAN
};

enum {
	IRCLIB_ERROR_DISCONNECTED,
	IRCLIB_ERROR_NICKNAME,
	IRCLIB_STATUS_CONNECTED
};

typedef struct Split {
	size_t          num;
	char       **data;
}               split_t;

typedef struct IRCLib {
#ifdef MACINTOSH_CLASSIC
	StreamPtr       s;
	int             readable;
#endif
#ifdef __MINGW32__
	WSADATA         wsadata;
	SOCKET          sock;
#else
	int             sock;
#endif
	unsigned char  *data;
	size_t          data_len;
	int             connected;
	void            (*callbacks[NUM_CALLBACKS]) (void *,...);
	char           *nickname;
	char           *realname;
	char	       *username;
	unsigned char  *buffered;
	size_t          waiting_len;
	int             isidle;
	int             isaway;
}
                IRCLIB;

typedef struct IRCLib_Handles {
	IRCLIB         *handle;
	struct IRCLib_Handles *next;
}
                IRCLIB_HANDLES;

typedef struct IRCLib_Packet {
	unsigned char  *data;
	size_t          len;
	size_t          offset;
}
                pkt_t;

#define HOST_BIG_ENDIAN 0
#define HOST_LITTLE_ENDIAN 1

#include "byteswap.h"
#include "protos.h"

#endif
