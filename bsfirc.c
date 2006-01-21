#include "bsfirc.h"
#include <signal.h>

#define chomp(x) { if(x[strlen(x)-2] == '\r') x[strlen(x)-2] = 0; else if(x[strlen(x)-1] == '\n') x[strlen(x)-1] = 0; }

char            inputbuf[513];
int             prompt_len;
struct BSFirc  *bsfirc;
struct ChannelList *chanlist = NULL;
struct Waiting *waiting = NULL;

int 
main(int argc, char **argv)
{
	char           *ircsrv, *ircnick, *ircname, *user;
	char           *p;
	int             portnumber;
	fd_set          readfs;
	struct timeval  tm;

	ircsrv = getenv("IRCSERVER");
	if (ircsrv == NULL) {
		printf("** IRC server: ");
		fflush(stdout);
		fgets(inputbuf, sizeof(inputbuf), stdin);
		chomp(inputbuf);
		ircsrv = strdup(inputbuf);
	}
	ircnick = getenv("IRCNICK");
	if (ircnick == NULL) {
		printf("** Nickname: ");
		fflush(stdout);
		fgets(inputbuf, sizeof(inputbuf), stdin);
		chomp(inputbuf);
		ircnick = strdup(inputbuf);
	}
	ircname = getenv("IRCNAME");
	if (ircname == NULL) {
		ircname = strdup("bsfirc user");
	}

	/*
	 * Read a port number, or use 6667 by default.
	 */
	if ((p = strchr(ircsrv, ':')) != NULL) {
		portnumber = atoi(&p[1]);
		if (portnumber < 1 || portnumber > 65535)
			portnumber = 6667; /* TODO: error message */
		*p = '\0';
	} else
		portnumber = 6667;

	setup_tty();
	get_screen_size();

	/*
	 * We don't like Ctrl-C
	 */
	signal(SIGINT, SIG_IGN);
#if !defined(__MINGW32__) && !defined(__DJGPP__)
#ifndef PLAN9
	signal(SIGWINCH, &sigwinch_handler);
#endif
	/*
	 * or Ctrl-Z in UNIX land
	 */
	signal(SIGTSTP, SIG_IGN);
#endif

	bsfirc = malloc(sizeof(struct BSFirc));
	bsfirc->istyping = 0;
	bsfirc->lastmsg = 0;
	bsfirc->lastchan = 0;
	bsfirc->ready = 0;
	bsfirc->server = NULL;
	bsfirc->lastmsgtype = LAST_MESSAGE_NONE;

	user = getenv("USER");

	if (user == NULL)
		user = strdup("bsfirc");

	bsfirc->handle = (void *) irclib_create_handle();
	irclib_setnick(bsfirc->handle, ircnick);
	irclib_setname(bsfirc->handle, ircname);
	irclib_setusername(bsfirc->handle, user);
	bsfirc->nick = strdup(ircnick);

	irclib_register_callback(bsfirc->handle, IRCLIB_MOTD, (void (*) (void *,...)) irc_motd);
	irclib_register_callback(bsfirc->handle, IRCLIB_READY, (void (*) (void *,...)) irc_ready);
	irclib_register_callback(bsfirc->handle, IRCLIB_JOIN, (void (*) (void *,...)) irc_join);
	irclib_register_callback(bsfirc->handle, IRCLIB_PART, (void (*) (void *,...)) irc_part);
	irclib_register_callback(bsfirc->handle, IRCLIB_PRIVMSG, (void (*) (void *,...)) irc_msg);
	irclib_register_callback(bsfirc->handle, IRCLIB_QUIT, (void (*) (void *,...)) irc_quit);
	irclib_register_callback(bsfirc->handle, IRCLIB_CHANUSER, (void (*) (void *,...)) irc_addchanuser);
	irclib_register_callback(bsfirc->handle, IRCLIB_NOTICE, (void (*) (void *,...)) irc_notice);
	irclib_register_callback(bsfirc->handle, IRCLIB_NAMESDONE, (void (*) (void *,...)) irc_namesdone);
	irclib_register_callback(bsfirc->handle, IRCLIB_NICKINUSE, (void (*) (void *,...)) irc_nickinuse);
	irclib_register_callback(bsfirc->handle, IRCLIB_CTCP, (void (*) (void *,...)) irc_ctcp);
	irclib_register_callback(bsfirc->handle, IRCLIB_MODE, (void (*) (void *,...)) irc_mode);
	irclib_register_callback(bsfirc->handle, IRCLIB_SERVER_NAME, (void (*) (void *,...)) irc_server_name);
	irclib_register_callback(bsfirc->handle, IRCLIB_TOPIC, (void (*) (void *,...)) irc_topic);
	irclib_register_callback(bsfirc->handle, IRCLIB_NOTICE_AUTH, (void (*) (void *,...)) irc_notice_auth);
	irclib_register_callback(bsfirc->handle, IRCLIB_WHOIS_USERHOST, (void (*) (void *,...)) irc_whois_userhost);
	irclib_register_callback(bsfirc->handle, IRCLIB_WHOIS_SERVER, (void (*) (void *,...)) irc_whois_server);
	irclib_register_callback(bsfirc->handle, IRCLIB_WHOIS_CHANNELS, (void (*) (void *,...)) irc_whois_channels);
	irclib_register_callback(bsfirc->handle, IRCLIB_AWAY, (void (*) (void *,...)) irc_away);
	irclib_register_callback(bsfirc->handle, IRCLIB_ERROR, (void (*) (void *,...)) error_callback);
	irclib_register_callback(bsfirc->handle, IRCLIB_NICKCHANGE, (void (*) (void *,...)) irc_nickchange);

	open_log_dir();

	printf("** bsfirc started.\n");
	printf("** Server set to %s, port %d.\n", ircsrv, portnumber);
	irclib_connect(bsfirc->handle, ircsrv, portnumber);

	while (!irclib_connected(bsfirc->handle));
	printf("** Connected.\n");

	memset(inputbuf, 0, sizeof(inputbuf));
	show_prompt();

	while (1) {
		tm.tv_sec = 2;
		tm.tv_usec = 500000;

		FD_ZERO(&readfs);
		FD_SET(0, &readfs);
		if (irclib_select(1, &readfs, NULL, NULL, &tm) != IRCLIB_RET_OK) {
			if (errno == EINTR)
				continue;
		}
		if (FD_ISSET(0, &readfs)) {
			get_input();
		}
	}
}

/* PROTO */
void
error_callback(void *handle, int code)
{
	eraseline();
	printf("** ");
	addts();
	putchar(' ');

	switch (code) {
	case IRCLIB_ERROR_DISCONNECTED:
		printf("Disconnected.\n");
		break;
	default:
		printf("Unknown error type %d\n", code);
	}

	show_prompt();
}
