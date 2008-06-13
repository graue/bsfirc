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

#define tok tokens->data

/* PROTO */
void
parse_message(void *handle, char *message)
{
	split_t		*tokens;
	int		numeric;

	tokens = i_split(message);

	if(strncmp(tok[0], "PING", 4) == 0) {
		pkt_t *pingpkt;

		pingpkt = pkt_init(5+strlen(tok[1]));
		pkt_addstr(pingpkt, "PONG ");
		pkt_addstr(pingpkt, tok[1]);
		send_cmdpkt(handle, pingpkt);
		pkt_free(pingpkt);
	} else if(strncmp(tok[0], "NOTICE", 6) == 0) {
		if(strncmp(tok[1], "AUTH", 4) == 0) {
			char *msgptr;
			msgptr = strchr(message, ':');
			if(((IRCLIB *)handle)->callbacks[IRCLIB_NOTICE_AUTH] != NULL)
				((IRCLIB *)handle)->callbacks[IRCLIB_NOTICE_AUTH](handle, msgptr+1);
		}
	} else if(message[0] == ':') {
		numeric = strtol(tok[1], (char **)NULL, 10);
		if(numeric == 0)
			parse_command(handle, message, tokens);
		else
			parse_numeric(handle, message, tokens, numeric);
	}

	i_free(tokens);
}

/* PROTO */
void
parse_command(void *handle, char *message, split_t *tokens)
{
	char           *nick = NULL, *host = NULL;
	char		*from = tok[0] + 1;
	size_t          len, toklen;
	int		argnum = 0, pos;

	if (strchr((char *) from, '!') != NULL) {
		len = chrdist((char *) from, '!');
		nick = xmalloc(len + 1);
		memcpy(nick, from, len);
		nick[len] = 0;
		host = strchr((char *) from, '!');
		host++;
	}

	if (strncmp(tok[1], "JOIN", 4) == 0) {
		char *chan;

		if(tok[2][0] == ':')
			chan = tolower_str(tok[2]+1);
		else
			chan = tolower_str(tok[2]);

		if (((IRCLIB *) handle)->callbacks[IRCLIB_JOIN] != NULL)
			((IRCLIB *) handle)->callbacks[IRCLIB_JOIN] (handle, nick, host, chan);
		free(chan);
	} else if (strncmp(tok[1], "PART", 4) == 0) {
		char *chan;

		if(tok[2][0] == ':')
			chan = tolower_str(tok[2]+1);
		else
			chan = tolower_str(tok[2]);

		if (((IRCLIB *) handle)->callbacks[IRCLIB_PART] != NULL)
			((IRCLIB *) handle)->callbacks[IRCLIB_PART] (handle, nick, host, chan);
		free(chan);
	} else if (strncmp(tok[1], "NOTICE", 6) == 0) {
		char *msgptr;
		
		/*
		 * see note on privmsg below
		 */
		msgptr = strchr(message, ' ');
		msgptr = strchr(msgptr+1, ':');

		if(((IRCLIB *) handle)->callbacks[IRCLIB_NOTICE] != NULL) {
			if(nick == NULL) {
				((IRCLIB *)handle)->callbacks[IRCLIB_NOTICE] (handle, from, NULL, msgptr+1);
			} else {
				((IRCLIB *)handle)->callbacks[IRCLIB_NOTICE] (handle, nick, host, msgptr+1);
			}
		}
	} else if (strncmp(tok[1], "PRIVMSG", 7) == 0) {
		char *msgptr;
		char *target;

		/*
		 * we have to do this, otherwise
		 * people with non-resolving IPv6 addresses
		 * screw us up.
		 */
		msgptr = strchr(message, ' ');
		msgptr = strchr(msgptr+1, ':');

		if(tok[2][0] == '#' || tok[2][0] == '&')
			target = tolower_str(tok[2]);
		else
			target = xstrdup(tok[2]);

		if(msgptr[1] == 0x01 && msgptr[strlen(msgptr)-1] == 0x01) {
			/*
			 * we should handle some CTCP stuff right here,
			 * not in the client
			 */
			msgptr[strlen(msgptr)-1] = 0;
			if(((IRCLIB *)handle)->callbacks[IRCLIB_CTCP] != NULL)
				((IRCLIB *)handle)->callbacks[IRCLIB_CTCP] (handle, nick, host, target, msgptr+2);
		} else { 
			if(((IRCLIB *) handle)->callbacks[IRCLIB_PRIVMSG] != NULL)
				((IRCLIB *)handle)->callbacks[IRCLIB_PRIVMSG] (handle, nick, host, target, msgptr+1);
		}

		free(target);
	} else if(strncmp(tok[1], "TOPIC", 5) == 0) {
		/*
		 * This is a topic change message. The topic notification
		 * on joining a new channel is a numeric (see below).
		 *
		 * TODO: make it a separate event which displays the nick.
		 */
		char *msgptr;

		msgptr = strchr(message, ' ');
		msgptr = strchr(msgptr+1, ':');
		
		if(((IRCLIB *)handle)->callbacks[IRCLIB_TOPIC] != NULL)
			((IRCLIB *)handle)->callbacks[IRCLIB_TOPIC] (handle, tok[2], msgptr+1);
	} else if(strncmp(tok[1], "NICK", 4) == 0) {
		if(((IRCLIB *)handle)->callbacks[IRCLIB_NICKCHANGE] != NULL)
			((IRCLIB *)handle)->callbacks[IRCLIB_NICKCHANGE] (handle, nick, tok[2]+1);
	} else if(strncmp(tok[1], "MODE", 4) == 0) {
		int plus = 1;
		char *fromptr;

		if(nick == NULL)
			fromptr = from;
		else
			fromptr = nick;

		if(tok[3][0] == ':')
			pos = 1;
		else
			pos = 0;

		toklen = strlen(tok[3]);

		for(; pos < toklen; pos++) {
			if(tok[3][pos] == '+') {
				plus = 1;
				continue;
			} else if(tok[3][pos] == '-') {
				plus = 0;
				continue;
			}

			if(tok[3][pos] == 'o') {
				if(((IRCLIB *)handle)->callbacks[IRCLIB_MODE] != NULL)
					((IRCLIB *)handle)->callbacks[IRCLIB_MODE] (handle, fromptr, host, tok[2], plus, C_MODE_OP, tok[4+argnum]);

				argnum++;
			}
			else if(tok[3][pos] == 'v') {
				if(((IRCLIB *)handle)->callbacks[IRCLIB_MODE] != NULL)
					((IRCLIB *)handle)->callbacks[IRCLIB_MODE] (handle, fromptr, host, tok[2], plus, C_MODE_VOICE, tok[4+argnum]);

				argnum++;
			}
		}
	} else if(strncmp(tok[1], "QUIT", 4) == 0) {
		char *msgptr;

		msgptr = strchr(message, ' ');
		msgptr = strchr(msgptr+1, ':');

		if(((IRCLIB *)handle)->callbacks[IRCLIB_QUIT] != NULL)
			((IRCLIB *)handle)->callbacks[IRCLIB_QUIT] (handle, nick, msgptr+1);
	}

	if (nick != NULL)
		free(nick);
}

/* PROTO */
void
parse_numeric(void *handle, char *message, split_t *tokens, int numeric)
{
	unsigned char *datastart;
	char *chan;
	size_t x;

	switch (numeric) {
	case 001: /* server name */
		if(((IRCLIB *)handle)->callbacks[IRCLIB_SERVER_NAME] != NULL)
			((IRCLIB *)handle)->callbacks[IRCLIB_SERVER_NAME] (handle, tok[0]+1);
		break;
	case 301: /* RPL_AWAY */
		datastart = (unsigned char *) strchr((char *) message + 1, ':');
		if (((IRCLIB *)handle)->callbacks[IRCLIB_AWAY] != NULL)
			((IRCLIB *)handle)->callbacks[IRCLIB_AWAY] (handle, tok[3], datastart+1);
		break;
	case 311: /* RPL_WHOISUSER */
		datastart = (unsigned char *) strchr((char *) message + 1, ':');
		if(((IRCLIB *)handle)->callbacks[IRCLIB_WHOIS_USERHOST] != NULL)
			((IRCLIB *)handle)->callbacks[IRCLIB_WHOIS_USERHOST] (handle, tok[3], tok[4], tok[5], datastart+1);
		break;
	case 312: /* RPL_WHOISSERVER */
		datastart = (unsigned char *) strchr((char *) message + 1, ':');
		if(((IRCLIB *)handle)->callbacks[IRCLIB_WHOIS_SERVER] != NULL)
			((IRCLIB *)handle)->callbacks[IRCLIB_WHOIS_SERVER] (handle, tok[3], tok[4], datastart+1);
		break;
	case 319: /* RPL_WHOISCHANNELS */
		datastart = (unsigned char *) strchr((char *) message + 1, ':');
		if(((IRCLIB *)handle)->callbacks[IRCLIB_WHOIS_CHANNELS] != NULL)
			((IRCLIB *)handle)->callbacks[IRCLIB_WHOIS_CHANNELS] (handle, tok[3], datastart+1);
		break;
	case 332: /* RPL_TOPIC */
		datastart = (unsigned char *) strchr((char *) message + 1, ':');

		if(((IRCLIB *)handle)->callbacks[IRCLIB_TOPIC] != NULL && datastart != NULL)
			((IRCLIB *)handle)->callbacks[IRCLIB_TOPIC] (handle, tok[3], datastart+1);

		break;
	case 353: /* RPL_NAMREPLY */
		chan = tok[4];
		for(x = 5; x < (tokens->num); x++) {
			if(tok[x][0] == '\0')
				continue;
			if(((IRCLIB *)handle)->callbacks[IRCLIB_CHANUSER] != NULL) {
				if(tok[x][0] == ':')
					((IRCLIB *)handle)->callbacks[IRCLIB_CHANUSER](handle, chan, tok[x]+1);
				else
					((IRCLIB *)handle)->callbacks[IRCLIB_CHANUSER](handle, chan, tok[x]);
			}
		}
		break;
	case 366: /* RPL_ENDOFNAMES */
		chan = tok[3];
		if(((IRCLIB *)handle)->callbacks[IRCLIB_NAMESDONE] != NULL)
			((IRCLIB *)handle)->callbacks[IRCLIB_NAMESDONE](handle, chan);
		break;
	case 372: /* RPL_MOTD */
	case 375: /* RPL_MOTDSTART */
	case 376: /* RPL_ENDOFMOTD */
	case 422: /* ERR_NOMOTD */
		datastart = (unsigned char *) strchr((char *) message + 1, ':');

		if (((IRCLIB *) handle)->callbacks[IRCLIB_MOTD] != NULL)
			((IRCLIB *) handle)->callbacks[IRCLIB_MOTD] (handle, datastart + 1);

		if (numeric == 422 || numeric == 376) {
			if (((IRCLIB *) handle)->callbacks[IRCLIB_READY] != NULL)
				((IRCLIB *) handle)->callbacks[IRCLIB_READY] (handle);
		}
		break;
	case 433: /* ERR_NICKNAMEINUSE */
		if(((IRCLIB *)handle)->callbacks[IRCLIB_NICKINUSE] != NULL)
			((IRCLIB *)handle)->callbacks[IRCLIB_NICKINUSE] (handle, tok[3]);
		break;
	default:
		break;
	}
}
