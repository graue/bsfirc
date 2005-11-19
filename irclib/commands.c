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

/* PROTO */
void
irclib_join(void *handle, char *channel)
{
	pkt_t          *pkt;

	pkt = pkt_init(5 + strlen(channel));

	pkt_addstr(pkt, "JOIN ");
	pkt_addstr(pkt, channel);

	send_cmdpkt(handle, pkt);
	pkt_free(pkt);
}

/* PROTO */
void
irclib_part(void *handle, char *channel)
{
	pkt_t *pkt;

	pkt = pkt_init(5 + strlen(channel));

	pkt_addstr(pkt, "PART ");
	pkt_addstr(pkt, channel);

	send_cmdpkt(handle, pkt);
	pkt_free(pkt);
}

/* PROTO */
void
irclib_privmsg(void *handle, char *target, char *message)
{
	pkt_t *pkt;

	pkt = pkt_init(10+strlen(target)+strlen(message));
	pkt_addstr(pkt, "PRIVMSG ");
	pkt_addstr(pkt, target);
	pkt_addstr(pkt, " :");
	pkt_addstr(pkt, message);
	
	send_cmdpkt(handle, pkt);
	pkt_free(pkt);
}

/* PROTO */
void
irclib_quit(void *handle, char *quitmsg)
{
	pkt_t *pkt;

	pkt = pkt_init(6+strlen(quitmsg));
	pkt_addstr(pkt, "QUIT :");
	pkt_addstr(pkt, quitmsg);

	send_cmdpkt(handle, pkt);
	pkt_free(pkt);
}

/* PROTO */
void
irclib_whois(void *handle, char *nick)
{
	pkt_t *pkt;

	pkt = pkt_init(6+strlen(nick));
	pkt_addstr(pkt, "WHOIS ");
	pkt_addstr(pkt, nick);

	send_cmdpkt(handle, pkt);
	pkt_free(pkt);
}

/* PROTO */
void
irclib_op(void *handle, char *chan, char *nick)
{
	pkt_t *pkt;

	pkt = pkt_init(9+strlen(chan)+strlen(nick));
	pkt_addstr(pkt, "MODE ");
	pkt_addstr(pkt, chan);
	pkt_addstr(pkt, " +o ");
	pkt_addstr(pkt, nick);

	send_cmdpkt(handle, pkt);
	pkt_free(pkt);
}
