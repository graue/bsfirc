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

#include "irclib.h"

/*
 * These are wrappers for send(), to make it
 * easier to port to other systems that don't use
 * BSD sockets (i.e. classic MacOS)
 */

/* PROTO */
ssize_t
socksend(void *handle, unsigned char *data, size_t len)
{
	ssize_t ret;

	ret =  send(((IRCLIB *) handle)->sock, data, len, 0);

	if(ret < 0) {
		if(((IRCLIB *)handle)->callbacks[IRCLIB_ERROR])
			((IRCLIB *)handle)->callbacks[IRCLIB_ERROR](handle, IRCLIB_ERROR_DISCONNECTED);
		((IRCLIB *)handle)->connected = 0;
		shutdown(((IRCLIB *)handle)->sock, 0x02);
		((IRCLIB *)handle)->sock = -1;
	} 

	return ret;
}

/* PROTO */
ssize_t
sendPkt(void *handle, pkt_t * packet)
{
	return socksend(handle, packet->data, packet->len);
}

/* PROTO */
ssize_t
send_cmdpkt(void *handle, pkt_t *packet)
{
	pkt_t *n;
	ssize_t ret;

	n = pkt_init(2+packet->len);
	pkt_addraw(n, packet->data, packet->len);
	pkt_addraw(n, (unsigned char *)"\r\n", 2);

	ret = sendPkt(handle, n);
	pkt_free(n);
	return ret;
}

/* PROTO */
size_t
chrdist(char *str, char ch)
{
	size_t          x;
	size_t          len = strlen(str);

	for (x = 0; x < len; x++) {
		if (str[x] == ch)
			return x;
	}

	return 0;
}

/* PROTO */
IRCLIB_RET
irclib_connect(void *handle, char *server, uint16_t port)
{
	IRCLIB         *hptr = (IRCLIB *) handle;
	pkt_t          *connectpkt;
	pkt_t          *nickpkt;
#ifdef MACINTOSH_CLASSIC

#else
	struct sockaddr_in sin;
	struct hostent *he;

	if ((he = gethostbyname(server)) == NULL) {
		perror("gethostbyname()");
		return IRCLIB_RET_ERROR;
	}
	if ((hptr->sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket()");
		return IRCLIB_RET_ERROR;
	}
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr = *((struct in_addr *) he->h_addr);
	memset(&(sin.sin_zero), 0, 8);

	if (connect(hptr->sock, (struct sockaddr *) & sin, sizeof(struct sockaddr)) == -1) {
		perror("connect()");
		return IRCLIB_RET_ERROR;
	}
#endif

	/* The hostname and servername sent here are typically unused.
	   However, if we send "x", some servers detect us as a spambot
	   and refuse the connection, so we reuse the username instead. */
	connectpkt = pkt_init(5 + strlen(hptr->username) + 1 +
			      strlen(hptr->username) + 1 +
			      strlen(hptr->username) + 1 +
			      strlen(hptr->realname) + 3);

	pkt_addstr(connectpkt, "USER ");
	pkt_addstr(connectpkt, hptr->username);
	pkt_addstr(connectpkt, " ");
	pkt_addstr(connectpkt, hptr->username); /* hostname */
	pkt_addstr(connectpkt, " ");
	pkt_addstr(connectpkt, hptr->username); /* servername */
	pkt_addstr(connectpkt, " :");
	pkt_addstr(connectpkt, hptr->realname);
	pkt_addstr(connectpkt, "\r\n");

	sendPkt(handle, connectpkt);

	pkt_free(connectpkt);

	nickpkt = pkt_init(7 + strlen(hptr->nickname));
	pkt_addstr(nickpkt, "NICK ");
	pkt_addstr(nickpkt, hptr->nickname);
	pkt_addstr(nickpkt, "\r\n");

	sendPkt(handle, nickpkt);
	pkt_free(nickpkt);

	hptr->connected = 1;
	return IRCLIB_RET_OK;
}
