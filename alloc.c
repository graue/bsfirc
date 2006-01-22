/*

        Error checking memory allocator

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef TESTERR
#undef NULL
#define NULL  buf
#endif

/*LINTLIBRARY*/

#define V        (void)

#ifdef SMARTALLOC

#include "smartall.h"

static void nomem(const char *fname, int lineno, size_t size)
{
	V fprintf(stderr, "\nBoom!!!  Memory capacity exceeded.\n");
	V fprintf(stderr, "  Requested %u bytes at line %d of %s.\n",
		size, lineno, fname);
	abort();
}

void *sm_xmalloc(char *fname, int lineno, size_t size)
{
	void *buf;
	buf = sm_malloc(fname, lineno, size);
	if (buf == NULL) nomem(fname, lineno, size);
	return buf;
}

void *sm_xrealloc(char *fname, int lineno, void *ptr, size_t size)
{
	void *buf;
	buf = sm_realloc(fname, lineno, ptr, size);
	if (buf == NULL) nomem(fname, lineno, size);
	return buf;
}

char *sm_xstrdup(char *fname, int lineno, const char *s)
{
	char *scopy;
	int slen = strlen(s);
	scopy = sm_xmalloc(fname, lineno, slen+1);
	memcpy(scopy, s, slen+1);
	return scopy;
}

#else /* not SMARTALLOC */

static void nomem(size_t size)
{
	V fprintf(stderr, "\nBoom!!!  Memory capacity exceeded.\n");
	V fprintf(stderr, "  Requested %u bytes.\n", size);
	abort();
}

void *xmalloc(size_t size)
{
	void *buf;
	buf = malloc(size);
	if (buf == NULL) nomem(size);
	return buf;
}

void *xrealloc(void *ptr, size_t size)
{
	void *buf;
	buf = realloc(ptr, size);
	if (buf == NULL) nomem(size);
	return buf;
}

char *xstrdup(const char *s)
{
	char *scopy;
	int slen = strlen(s);
	scopy = xmalloc(slen+1);
	memcpy(scopy, s, slen+1);
	return scopy;
}

#endif
