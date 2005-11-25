#include "bsfirc.h"
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>
#ifdef PLAN9
#include <time.h>
#endif

#ifdef __linux__
#include <linux/limits.h>
#endif

#ifdef __sun
#ifndef PATH_MAX
#define PATH_MAX 1024
#endif
#endif

#ifdef __DJGPP__
#define PATH_MAX 256		/* this is a guess */
#endif

#ifdef PLAN9
#define PATH_MAX 1024		/* again, a guess */
#endif

char            logpath[PATH_MAX];
int             logging;

/* PROTO */
int
open_log_dir(void)
{
	DIR            *tmp;
#if !defined(__MINGW32__) && !defined(__DJGPP__)
	char           *homedir;
#endif

	logging = 1;

#if !defined(__MINGW32__) && !defined(__DJGPP__)

#ifdef PLAN9
	homedir = getenv("home");
	sprintf(logpath, "%s/lib/bsfirc", homedir);
	mkdir(logpath, 0777);
	sprintf(logpath, "%s/lib/bsfirc/log", homedir);
#else
	homedir = getenv("HOME");
	
	sprintf(logpath, "%s/.bsfirc", homedir);
	umask(077);
	mkdir(logpath, 0777);
	sprintf(logpath, "%s/.bsfirc/log", homedir);
#endif				/* PLAN9 */

#else
	sprintf(logpath, "log");
#endif
	tmp = opendir(logpath);
	if (tmp == NULL) {
#ifndef __MINGW32__
		if (mkdir(logpath, 0777) == -1) {
#else
		if (mkdir(logpath) == -1) {
#endif
			perror("Couldn't make log directory: ");
			logging = 0;
			return -1;
		}
		return 0;
	} else
		closedir(tmp);

	return 0;
}

/* PROTO */
void
log_event(int event_type, char *name, char *host, char *chan, char *msg)
{
	FILE           *logfile;
	char            user_log[PATH_MAX];
	char            lcname[512];
	char            ts[21];
	struct tm      *now;
	time_t          t;
	int             i;

	if (!logging)
		return;

	if(event_type == EVENT_CHANMSG ||
           event_type == EVENT_CHANJOIN ||
	   event_type == EVENT_CHANPART ||
	   event_type == EVENT_QUIT) {
		for (i = 0; chan[i] && i < sizeof (lcname) - 1; i++)
			lcname[i] = tolower(chan[i]);
		lcname[i] = '\0';
		sprintf(user_log, "%s/%s.log", logpath, lcname);
	} else {
		for (i = 0; name[i] && i < sizeof (lcname) - 1; i++)
			lcname[i] = tolower(name[i]);
		lcname[i] = '\0';
		sprintf(user_log, "%s/%s.log", logpath, name);
	}

	logfile = fopen(user_log, "a");
	if (logfile == NULL)
		return;

	t = time(0);
	now = localtime(&t);
	strftime(ts, 20, "%m/%d/%Y %H:%M:%S", now);

	switch (event_type) {
	case EVENT_PRIVMSG:
		fprintf(logfile, "%s: <- %s\n", ts, msg);
		break;
	case EVENT_CHANJOIN:
		fprintf(logfile, "%s: %s (%s) joined %s.\n", ts, name, host, chan);
		break;
	case EVENT_CHANPART:
		fprintf(logfile, "%s: %s (%s) left %s.\n", ts, name, host, chan);
		break;
	case EVENT_CHANMSG:
		fprintf(logfile, "%s: <%s> %s\n", ts, name, msg);
		break;
	case EVENT_QUIT:
		fprintf(logfile, "%s: %s has quit: %s\n", ts, name, msg);
		break;
	case EVENT_SENDPRIVMSG:
		fprintf(logfile, "%s: -> %s\n", ts, msg);
		break;
	}

	fclose(logfile);
	return;
}

/* PROTO */
void
show_log(int lines, char *dest)
{
	FILE           *logfile;
	char            user_log[PATH_MAX];
	char            buf[1024];
	char            tmp[25];
	int             ch;
	int             linect = 0, linect2 = 0;

	sprintf(user_log, "%s/%s.log", logpath, dest);
	putchar('\n');

	logfile = fopen(user_log, "r");
	if (logfile == NULL)
		return;

	while (!feof(logfile)) {
		ch = fgetc(logfile);
		if (ch == '\n')
			linect++;
	}

	rewind(logfile);

	if (linect > lines) {
		while (!feof(logfile)) {
			ch = fgetc(logfile);
			if (ch == '\n') {
				linect2++;
				if (linect2 == (linect - lines))
					break;
			}
		}
	}
	while (!feof(logfile)) {
		if (fgets(buf, sizeof(buf), logfile) == NULL)
			break;
		memset(tmp, 0, sizeof(tmp));
		strncpy(tmp, buf, 24);
		printf("%s", tmp);
		if (buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = 0;
		wordwrap_print(buf + 24, 24);
	}

	fclose(logfile);
}
