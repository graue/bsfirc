#include "bsfirc.h"

#ifdef PLAN9
#include  <time.h>
#endif

extern int prompt_len;
extern char inputbuf[513];
extern struct Waiting *waiting;

/* PROTO */
void
addts(void)
{
	char ts[11];
	struct tm *now;
	time_t t;

	t = time(0);
	now = localtime(&t);
	strftime(ts, 11, "[%H:%M:%S]", now);
	printf("%s", ts);
}

/* PROTO */
void
addts_short(void)
{
	char ts[6];
	struct tm *now;
	time_t t;

	t = time(0);
	now = localtime(&t);
	strftime(ts, 6, "%H:%M", now);
	printf("%s", ts);
}

/* PROTO */
void
eraseline(void)
{
	int x, l = strlen(inputbuf) + prompt_len;

	for(x = 0; x < l; x++)
		printf("\b \b");
}

/* PROTO */
void
show_prompt(void)
{
	struct Waiting *wtr;

	prompt_len = strlen(BSF_PROMPT) + 1;

	if(waiting != NULL) {
		for(wtr = waiting; wtr != NULL; wtr = wtr->next) {
			prompt_len += strlen(wtr->nick) + 3;
			printf("[%s] ", wtr->nick);
		}
	}

	printf("%s", BSF_PROMPT);
	printf(" %s", inputbuf);
	fflush(stdout);
}
