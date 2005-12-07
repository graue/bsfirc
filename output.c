#include "bsfirc.h"

#include <stdio.h>
#include <sys/time.h>
#ifdef PLAN9
#include <time.h>
#endif
#include <stdlib.h>
#include <string.h>

#ifdef __MINGW32__
#include <windows.h>
#else
#include <termios.h>
#include <sys/ioctl.h>
#endif

#ifdef __DJGPP__
#include <conio.h>
#endif

#ifdef __MINGW32__
HANDLE          hOut;
#endif

#if !defined(PLAN9)
#define TERMINAL_HIJINX
#endif

#ifdef TERMINAL_HIJINX

#ifdef __MINGW32__
#define TERMINAL_WINDOWS
#else

#ifdef __DJGPP__
#define TERMINAL_CONIO
#else
#define TERMINAL_VT100
#endif

#endif

#endif

extern int screen_cols;
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
	int             linelen = strlen(inputbuf) + prompt_len;
#ifdef TERMINAL_HIJINX
	int             numcols, numrows, x;
#ifdef TERMINAL_CONIO
	int             desiredx, desiredy;
#endif

#ifdef TERMINAL_WINDOWS
	COORD           nPos;
	CONSOLE_SCREEN_BUFFER_INFO wInfo;
	get_screen_size();
#endif

	numrows = linelen / screen_cols;
	numcols = linelen % screen_cols;

#ifdef TERMINAL_VT100
	if (numrows > 0)
		printf("\033[%dA", numrows);

	if (numcols > 0)
		printf("\033[%dD", numcols);
#endif
#ifdef TERMINAL_WINDOWS
	GetConsoleScreenBufferInfo(hOut, &wInfo);

	nPos.X = wInfo.dwCursorPosition.X - numcols;
	nPos.Y = wInfo.dwCursorPosition.Y - numrows;

	SetConsoleCursorPosition(hOut, nPos);
#endif

#ifdef TERMINAL_CONIO
	desiredx = wherex() - numcols;
	desiredy = wherey() - numrows;

	gotoxy(desiredx, desiredy);
#endif

	for (x = 0; x < linelen; x++)
		putchar(' ');

#ifdef TERMINAL_VT100
	if (numrows > 0)
		printf("\033[%dA", numrows);

	if (numcols > 0)
		printf("\033[%dD", numcols);
#endif
#ifdef TERMINAL_WINDOWS
	GetConsoleScreenBufferInfo(hOut, &wInfo);

	nPos.X = wInfo.dwCursorPosition.X - numcols;
	nPos.Y = wInfo.dwCursorPosition.Y - numrows;

	SetConsoleCursorPosition(hOut, nPos);
#endif
#ifdef TERMINAL_CONIO
	fflush(stdout);

	gotoxy(desiredx, desiredy);
#endif

#else
	int             x;

#ifdef PLAN9
	get_screen_size();
#endif

	for (x = 0; x < linelen; x++)
		printf("\b \b");
#endif

#ifdef COLOR
	set_color(0);
#endif
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
