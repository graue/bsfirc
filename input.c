#include "bsfirc.h"

extern char     inputbuf[513];
extern struct BSFirc *bsfirc;
extern struct Waiting *waiting;
extern struct ChannelList *chanlist;

/* PROTO */
void
get_input(void)
{
	unsigned char   inchr;
	struct Waiting *wtmp, *wtmp2;
	struct ChannelList *tr;
	struct UserList *utr;
	int             match = 0;

	read(0, &inchr, 1);

	switch (inchr) {
	case '\n':
	case '\r':
		parse_input();
		memset(inputbuf, 0, sizeof(inputbuf));
		bsfirc->istyping = 0;

		for (wtmp = waiting; wtmp != NULL;) {
			wtmp2 = wtmp;
			wtmp = wtmp->next;
			free(wtmp2->nick);
			free(wtmp2);
		}

		waiting = NULL;

		show_prompt();
		break;
	case 21:
		eraseline();
		memset(inputbuf, 0, sizeof(inputbuf));
		bsfirc->istyping = 0;
		show_prompt();
		break;
	case 12:

#if !defined(__MINGW32__) && !defined(PLAN9)
#ifdef __DJGPP__
		clrscr();
#else
		printf("\033[2J\033[1;1H");
#endif
		show_prompt();
#endif
		break;
	case '\b':
	case 127:
	case 4:
		if (inputbuf[0] == 0) {
			bsfirc->istyping = 0;
			break;
		}
		printf("\b \b");
		inputbuf[strlen(inputbuf) - 1] = 0;
		fflush(stdout);
		break;
	case 'r':
	case '\'':
	case 'c':
	case '\t':
		if (inputbuf[0] == 0) {
			if (inchr == 'r') {
				if (bsfirc->lastmsgtype == LAST_MESSAGE_CHANNEL) {
					if(bsfirc->lastchan) {
						sprintf(inputbuf, "m%s ", bsfirc->lastchan);
					}
				} else if(bsfirc->lastmsgtype == LAST_MESSAGE_PRIVATE) {
					if(bsfirc->lastmsg) {
						sprintf(inputbuf, "m%s ", bsfirc->lastmsg);
					}
				}
				
				if(bsfirc->lastmsgtype != LAST_MESSAGE_NONE) {
					printf("%s", inputbuf);
					fflush(stdout);
					bsfirc->istyping = 1;
				}
			} else if (inchr == '\t') {
				if (bsfirc->lastmsg) {
					printf("m%s ", bsfirc->lastmsg);
					bsfirc->istyping = 1;
					sprintf(inputbuf, "m%s ", bsfirc->lastmsg);
					fflush(stdout);
				}
			} else if (inchr == 'c' || inchr == '\'') {
				if (bsfirc->lastchan) {
					printf("m%s ", bsfirc->lastchan);
					bsfirc->istyping = 1;
					sprintf(inputbuf, "m%s ", bsfirc->lastchan);
					fflush(stdout);
				}
			}
			break;
		} else if (inchr == '\t') {
			switch (inputbuf[0]) {
			case 'i':
			case 'm':
				if (strchr(inputbuf, ' ') != NULL)
					break;

				for (tr = chanlist; tr != NULL; tr = tr->next) {
					if (strcasecmp(tr->chan, bsfirc->lastchan) == 0) {
						for (utr = tr->users; utr != NULL; utr = utr->next) {
							if (strncasecmp(utr->name, inputbuf + 1, strlen(inputbuf) - 1) == 0) {
								match++;
							}
						}

						if (match > 1 || match == 0)
							break;
						else {
							for (utr = tr->users; utr != NULL; utr = utr->next) {
								if (strncasecmp(utr->name, inputbuf + 1, strlen(inputbuf) - 1) == 0)
									break;
							}

							eraseline();
							memcpy(inputbuf + 1, utr->name, strlen(utr->name));

							if (inputbuf[0] == 'm')
								inputbuf[strlen(inputbuf)] = ' ';

							show_prompt();

						}
						break;
					}
				}
			}
		}
	default:
		if (inchr < 32)
			break;

		bsfirc->istyping = 1;

		if (strlen(inputbuf) == 512)
			break;

		inputbuf[strlen(inputbuf)] = inchr;
		putchar(inchr);
		fflush(stdout);
	}
}

/* PROTO */
void
parse_input(void)
{
	if (strncasecmp(inputbuf, "q!", 2) == 0) {
		irclib_quit(bsfirc->handle,
			inputbuf[2] ? &inputbuf[2] : "Leaving");
		restore_tty();
		printf("\n");
		if (bsfirc->nick)     free(bsfirc->nick);
		if (bsfirc->server)   free(bsfirc->server);
		if (bsfirc->lastchan) free(bsfirc->lastchan);
		if (bsfirc->lastmsg)  free(bsfirc->lastmsg);
		free_lists();
		sm_dump(1);
		exit(-1);
	} else if (inputbuf[0] == 'i') {
		irclib_whois(bsfirc->handle, inputbuf + 1);
	} else if (inputbuf[0] == 'm') {
		char           *pptr, *pptr2, *dest;
		int             offset;
#ifdef NETSPEAK_CLEANER
		char           *cleanmsg;
#endif

		pptr2 = inputbuf + 1;

		pptr = strchr(pptr2, ' ');
		if (pptr == NULL) {
			printf("\nNo message to send.\n");
			return;
		}
		dest = xmalloc(pptr - pptr2 + 1);
		dest[pptr - pptr2] = 0;
		strncpy(dest, pptr2, pptr - pptr2);
		irclib_privmsg(bsfirc->handle, dest, pptr + 1);

		if (dest[0] == '#' || dest[0] == '&') {
			if (bsfirc->lastchan != NULL)
				free(bsfirc->lastchan);
			bsfirc->lastchan = xstrdup(dest);
			bsfirc->lastmsgtype = LAST_MESSAGE_CHANNEL;
		} else {
			if (bsfirc->lastmsg != NULL)
				free(bsfirc->lastmsg);
			bsfirc->lastmsg = xstrdup(dest);
			bsfirc->lastmsgtype = LAST_MESSAGE_PRIVATE;
		}

		eraseline();
		if (dest[0] == '#' || dest[0] == '&') {
			putchar('[');
#ifdef TIMESTAMPS_CHANMSG
			addts_short();
			putchar('/');
			offset = 6;
#else
			offset = 0;
#endif
			printf("%s] (%s) ", dest, bsfirc->nick);
			offset += strlen(dest) + strlen(bsfirc->nick) + 6;
#ifdef NETSPEAK_CLEANER
			cleanmsg = undo_netspeak(pptr + 1);
			wordwrap_print(cleanmsg, offset);
			free(cleanmsg);
#else
			wordwrap_print(pptr + 1, offset);
#endif
			log_event(EVENT_CHANMSG, bsfirc->nick, NULL, dest, pptr + 1);
		} else {
#ifdef TIMESTAMPS
			addts();
			putchar(' ');
			offset = 13;
#else
			offset = 2;
#endif

			offset += strlen(dest) + 2;
			printf("->%s", dest);
			printf(": ");
#ifdef NETSPEAK_CLEANER
			cleanmsg = undo_netspeak(pptr + 1);
			wordwrap_print(cleanmsg, offset);
			free(cleanmsg);
#else
			wordwrap_print(pptr + 1, offset);
#endif
			log_event(EVENT_SENDPRIVMSG, dest, NULL, NULL, pptr+1);
		}

		free(dest);
		return;
	} else if (inputbuf[0] == 'j') {
		char *key;
		char *chan;

		key = chan = inputbuf+1;
		while (*key && !isspace(*key))
			key++;
		if (key == '\0')
			key = NULL;
		else {
			*key = '\0';
			key++;
			while (isspace(*key))
				key++;
			if (*key == '\0')
				key = NULL;
		}
		irclib_join(bsfirc->handle, chan, key);
	} else if (inputbuf[0] == 'n') {
		/*
		 * Keep bsfirc->nick the same for now, since the nick
		 * change might fail. It isn't effective until the server
		 * tells us.
		 */
		irclib_setnick(bsfirc->handle, inputbuf+1);
	} else if (inputbuf[0] == 'p') {
		irclib_part(bsfirc->handle, inputbuf + 1);
	} else if (inputbuf[0] == 'w') {
		if (inputbuf[1] == 0) {
			if (bsfirc->lastchan != NULL) {
				printf("\n");
				show_channel_users(bsfirc->lastchan);
				return;
			}
		} else {
			printf("\n");
			show_channel_users(inputbuf + 1);
			return;
		}
	} else if (inputbuf[0] == 't') {
		irclib_topic(bsfirc->handle, bsfirc->lastchan, inputbuf + 1);
	} else if (inputbuf[0] == 'a') {
		irclib_away(bsfirc->handle, inputbuf + 1);
	} else if (inputbuf[0] == 'o') {
		irclib_op(bsfirc->handle, bsfirc->lastchan, inputbuf + 1);
	} else if (inputbuf[0] == 'W') {
		printf("\n** ");
		addts();
		printf(" %s", bsfirc->nick);
		if (bsfirc->server != NULL)
			printf(" on %s", bsfirc->server);
	} else if (inputbuf[0] == '?' || inputbuf[0] == 'h') {
		printf("\n** bsfirc commands:\n");
		printf("   j<chan>        : join <chan>\n");
		printf("   p<chan>        : leave <chan>\n");
		printf("   m<rec> <msg>   : send <msg> to <rec> (channel or user)\n");
		printf("   c<msg>         : reply to last channel message\n");
		printf("   '<msg>         : same as c\n");
		printf("   [TAB]<msg>     : reply to last private message\n");
		printf("   r<msg>         : reply to last message\n");
		printf("   w              : show who is in the channel\n");
		printf("   i<nick>        : whois <nick>\n");
		printf("   W              : show your nickname and server\n");
		printf("   t<msg>         : set topic on current channel\n");
		printf("   a<msg>         : set away message\n");
		printf("   o<nick>        : op <nick> on current channel\n");
		printf("   n<nick>        : change your nick to <nick>\n");
		printf("   q!             : quit");
	}
	printf("\n");
}
