#include "bsfirc.h"

#include <stdio.h>
#include <sys/time.h>

#ifdef PLAN9
#include <time.h>
#include <fcntl.h>
#endif

#ifndef __MINGW32__
#include <sys/ioctl.h>
#include <termios.h>
#else
#include <conio.h>
#endif

#ifndef __MINGW32__
struct termios t_attr;
struct termios saved_attr;
int istat, attrs_saved;
#endif

int screen_cols;
int screen_lines;
int prompt_len;

/* PROTO */
void
sigwinch_handler(int a)
{
	get_screen_size();
}

/* PROTO */
void
setup_tty(void)
{
#if !defined(__MINGW32__) && !defined(PLAN9)
	istat = 0;
	if (tcgetattr(fileno(stdin), &t_attr) != 0)
		return;
	if (!attrs_saved) {
		saved_attr = t_attr;
		attrs_saved = 1;
	}
	t_attr.c_lflag &= ~(ECHO | ICANON);
	t_attr.c_cc[VMIN] = 1;
	if (tcsetattr(fileno(stdin), TCSAFLUSH, &t_attr) != 0)
		perror("can't change tty modes.");
#else
#ifdef PLAN9
	int             consctl;

	consctl = open("/dev/consctl", O_WRONLY);
	if (consctl < 0) {
		perror("Can't open consctl");
		exit(-1);
	}
	write(consctl, "rawon", 5);
#endif
#endif
}

/* PROTO */
void
restore_tty(void)
{
#ifndef __MINGW32__
	tcsetattr(fileno(stdin), TCSAFLUSH, &saved_attr);
#endif
}

/* PROTO */
void
get_screen_size(void)
{
#if defined(__DJGPP__) || defined(__MINGW32__)
	screen_cols = 80;
	screen_lines = 25;
#elif defined (PLAN9)
	screen_cols = getwidth();
	screen_lines = 25;
#else
	struct winsize scrsize;

	ioctl(fileno(stdin), TIOCGWINSZ, &scrsize);

	screen_cols = scrsize.ws_col;
	screen_lines = scrsize.ws_row;
#endif
}


/* PROTO */
void
wordwrap_print(char *str, int offset)
{
	char           *linebuf;
	char           *curline;
	int             firstline = 1;
	int             xx = 0, yy, jj;

	linebuf = malloc(screen_cols + 1);
	curline = str;

	for (;;) {
		if (curline[xx] == 0)
			break;
		if (curline[xx] != ' ') {
			xx++;
			continue;
		}
		yy = xx + 1;
		while (curline[yy] != ' ')
			if (curline[yy] == 0)
				break;
			else
				yy++;

		if (yy > (screen_cols - offset - 1)) {
			memset(linebuf, 0, screen_cols + 1);
			strncpy(linebuf, curline, xx);
			if (!firstline) {
				for (jj = 0; jj < offset; jj++)
					putchar(' ');
			} else {
				firstline = 0;
			}

			printf("%s\n", linebuf);
			curline += xx + 1;
			xx = 0;
			continue;
		}
		xx++;
	}

	if (!firstline)
		for (jj = 0; jj < offset; jj++)
			putchar(' ');

	printf("%s\n", curline);
	free(linebuf);
}

/* PROTO */
void
wordwrap_print_echostr(char *str, char *echostr)
{
	char           *linebuf;
	char           *curline;
	int             offset;
	int             xx = 0, yy;

	offset = strlen(echostr) + 1;
	linebuf = malloc(screen_cols + 1);
	curline = str;

	for (;;) {
		if (curline[xx] == 0)
			break;
		if (curline[xx] != ' ') {
			xx++;
			continue;
		}
		yy = xx + 1;
		while (curline[yy] != ' ')
			if (curline[yy] == 0)
				break;
			else
				yy++;

		if (yy > (screen_cols - offset - 1)) {
			memset(linebuf, 0, screen_cols + 1);
			strncpy(linebuf, curline, xx);
			printf("%s %s\n", echostr, linebuf);
			curline += xx + 1;
			xx = 0;
			continue;
		}
		xx++;
	}

	printf("%s %s\n", echostr, curline);
	free(linebuf);
}

