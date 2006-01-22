/* xmalloc, xrealloc, and strdup routines */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define V (void)

static void
nomem(size_t size)
{
	V fprintf(stderr, "\nBoom!!!  Memory capacity exceeded.\n");
	V fprintf(stderr, "  Requested %lu bytes.\n", (unsigned long) size);
	abort();
}

/* PROTO */
void *
xmalloc(size_t size)
{
	void *buf;
	buf = malloc(size);
	if (buf == NULL) nomem(size);
	return buf;
}

/* PROTO */
void *
xrealloc(void *ptr, size_t size)
{
	void *buf;
	buf = realloc(ptr, size);
	if (buf == NULL) nomem(size);
	return buf;
}

/* PROTO */
char *
xstrdup(const char *s)
{
	char *scopy;
	int slen = strlen(s);
	scopy = xmalloc(slen+1);
	memcpy(scopy, s, slen+1);
	return scopy;
}
