#include "bsfirc.h"

extern struct BSFirc *bsfirc;
extern struct ChannelList *chanlist;

/* PROTO */
void
free_lists(void)
{
	struct ChannelList *tr, *next;
	struct UserList *utr, *unext;

	for (tr = chanlist, next = (tr ? tr->next : NULL);
		tr != NULL;
		tr = next, next = (tr ? tr->next : NULL))
	{
		for (utr = tr->users, unext = (utr ? utr->next : NULL);
			utr != NULL;
			utr = unext, unext = (utr ? utr->next : NULL))
		{
			if (utr->name) free(utr->name);
			free(utr);
		}
		if (tr->chan != NULL) free(tr->chan);
		free(tr);
	}

	chanlist = NULL;
}

/* PROTO */
void
show_channel_users(char *chan)
{
	struct ChannelList *tr;
	struct UserList *utr;
	int col;
	uint8_t ch;

	printf("** ");

	for(tr = chanlist; tr != NULL; tr = tr->next) {
		if(strcasecmp(tr->chan, chan) == 0) {
			addts();
			printf(" Users in %s:\n", tr->chan);
			for(utr = tr->users, col = 0; utr != NULL; utr = utr->next) {
				if(utr->mode == MODE_OP)
					ch = '@';
				else if(utr->mode == MODE_VOICE)
					ch = '+';
				else
					ch = '.';

				printf("%s [%c]", USERLIST_ECHOSTR, ch);
				if(col < 2) {
					printf(" %-16s\t", utr->name);
					col++;
				} else {
					printf(" %s\n", utr->name);
					col = 0;
				}
			}

			if(col != 0)
				printf("\n");
				
			break;
		}
	}
}

/* PROTO */
void
delete_channel_user(char *name, char *chan)
{
	struct ChannelList *tr, *clist = NULL;
	struct UserList *utr, *tmp;

	for(tr = chanlist; tr != NULL; tr = tr->next) {
		if(strcasecmp(chan, tr->chan) == 0) {
			clist = tr;
			break;
		}
	}

	/* If the channel is not in our list, nothing to do. */
	if (clist == NULL)
		return;

	if(strcasecmp(clist->users->name, name) == 0) {
		tmp = clist->users;
		clist->users = clist->users->next;
		free(tmp->name);
		free(tmp);
	} else {
		for(utr = clist->users; utr->next != NULL; utr = utr->next) {
			if(strcasecmp(utr->next->name, name) == 0) {
				tmp = utr->next;
				utr->next = utr->next->next;
				free(tmp->name);
				free(tmp);
				break;
			}
		}
	}
}

/* PROTO */
void
process_quit(char *name, char *msg)
{
	struct ChannelList *tr;
	struct UserList *utr, *tmp;

	for(tr = chanlist; tr != NULL; tr = tr->next) {
		if(strcasecmp(tr->users->name, name) == 0) {
			tmp = tr->users;
			tr->users = tr->users->next;
			free(tmp->name);
			free(tmp);
			log_event(EVENT_QUIT, name, NULL, tr->chan, msg);
		} else {
			for(utr = tr->users; utr->next != NULL; utr = utr->next) {
				if(strcasecmp(utr->next->name, name) == 0) {
					tmp = utr->next;
					utr->next = utr->next->next;
					free(tmp->name);
					free(tmp);
					log_event(EVENT_QUIT, name, NULL, tr->chan, msg);
					break;
				}
			}
		}
	}
}

/* PROTO */
void
change_user_mode(char *name, char *chan, int plus, int mode)
{
	struct ChannelList *tr;
	struct UserList *utr;

	for(tr = chanlist; tr != NULL; tr = tr->next) {
		if(strcasecmp(chan, tr->chan) == 0) {
			for(utr = tr->users; utr != NULL; utr = utr->next) {
				if(strcasecmp(name, utr->name) == 0) {
					if(mode == C_MODE_OP) {
						if(plus == 1) 
							utr->mode |= MODE_OP;
						else
							utr->mode &= ~MODE_OP;
					}

					break;
				}
			}
		}
	}
}

/* PROTO */
void
change_user_nick(char *old, char *new)
{
	struct ChannelList *tr;
	struct UserList *utr;

	for(tr = chanlist; tr != NULL; tr = tr->next) {
		for(utr = tr->users; utr != NULL; utr = utr->next) {
			if(strcasecmp(old, utr->name) == 0) {
				free(utr->name);
				utr->name = strdup(new);
				break;
			}
		}
	}

	/* If it's me, I need to know what my new nick is. */
	if (strcasecmp(old, bsfirc->nick) == 0) {
		free(bsfirc->nick);
		bsfirc->nick = strdup(new);
	}
}

/* PROTO */
void
add_channel_user(char *name, char *chan, uint8_t mode)
{
	struct ChannelList *tr, *p;
	struct UserList *up, *utr;

	if(chanlist == NULL) {
		chanlist = malloc(sizeof(struct ChannelList));
		chanlist->chan = strdup(chan);
		chanlist->next = NULL;
		chanlist->users = NULL;
		p = chanlist;
	} else {
		p = NULL;

		for(tr = chanlist; tr != NULL; tr = tr->next) {
			if(strcasecmp(chan, tr->chan) == 0) {
				p = tr;
				break;
			}
		}

		if(p == NULL) {
			for(tr = chanlist; tr->next != NULL; tr = tr->next);
			tr->next = malloc(sizeof(struct ChannelList));
			tr->next->chan = strdup(chan);
			tr->next->next = NULL;
			tr->next->users = NULL;
			p = tr->next;
		}
	}

	if(p->users == NULL) {
		p->users = malloc(sizeof(struct UserList));
		up = p->users;
		up->next = NULL;
	} else {
		for(utr = p->users; utr != NULL; utr = utr->next) {
			if(strcasecmp(utr->name, name) == 0)
				return;
		}

		for(utr = p->users; utr != NULL; utr = utr->next)
			if(strcasecmp(name, utr->name) < 0)
				break;

		up = malloc(sizeof(struct UserList));

		if(utr == NULL) {
			for(utr = p->users; utr->next != NULL;
					 utr = utr->next);
			up->next = NULL;
			utr->next = up;
		} else {
			if(utr == p->users) {
				up->next = p->users;
				p->users = up;
			} else {
				struct UserList *utr2;
				up->next = utr;
				for(utr2 = p->users; utr2->next != utr;
						utr2 = utr2->next);
				utr2->next = up;
			}
		}
	}

	up->name = strdup(name);
	up->mode = mode;
}
