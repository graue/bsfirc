/**
 ** bsflite - bs-free AIM client
 **           ultralight version.
 **
 ** (C) 2003-2005 by Claudio Leite <leitec at leitec dot org>
 **
 ** NO WARRANTY. Read the file COPYING for more details.
 **/

#include "bsfirc.h"

#ifdef NETSPEAK_CLEANER

/**
 ** This is probably not the right way to do it.
 **
 ** Whenever I have to do anything like this, I really
 ** begin to wish I had taken computer science classes.
 **
 ** It works, however.
 **/

struct WordTable {
	char           *orig;
	char           *clean;
};

const struct WordTable table[] = {
	{"u", "you"},
	{"r", "are"},
	{"i", "I"},
	{"i'm", "I'm"},
	{"i've", "I've"},
	{"ive", "I've"},
	{"i'd", "I'd"},
	{"ur", "you're"},	/* there's no way to remove the ambiguity;
				 * pick one. */
	{"ne1", "anyone"},
	{"omg", "oh my god,"},
	{"teh", "the"},
	{"taht", "that"},
	{"funnay", "funny"},
	{"d00d", "dude"},
	{"dood", "dude"},
	{"liek", "like"},
	{"osmething", "something"},
	{NULL, NULL}
};

/* PROTO */
char           *
undo_netspeak(char *orig)
{
	char           *cleaned, *spcptr = orig;
	size_t          x, y, newlen, origlen = strlen(orig);
	int             found;

	newlen = origlen;

	while (1) {
		for (x = 0; table[x].orig != NULL; x++) {
			int             len = strlen(table[x].orig);
			if (strncmp(table[x].orig, spcptr, len) == 0) {
				if (spcptr[len] == ' ' || spcptr[len] == 0 || spcptr[len] == '.' || spcptr[len] == ',') {
					newlen += (ssize_t) strlen(table[x].clean) - len;
					break;
				}
			}
		}

		spcptr = strchr(spcptr, ' ');
		if (spcptr == NULL)
			break;

		spcptr++;
	}

	cleaned = malloc(newlen + 1);
	cleaned[0] = 0;

	for (x = 0; x < origlen; x++) {
		found = 0;

		if (x > 0) {
			if (orig[x - 1] != ' ') {
				strncat(cleaned, &orig[x], 1);
				continue;
			}
		}
		for (y = 0; table[y].orig != NULL; y++) {
			int             len = strlen(table[y].orig);

			if (orig[x + len] != ' ' && orig[x + len] != '.' && orig[x + len] != 0 && orig[x + len] != ',')
				continue;

			if (strncmp(table[y].orig, orig + x, len) == 0) {
				strcat(cleaned, table[y].clean);
				x += len - 1;
				found = 1;
				break;
			}
		}

		if (found)
			continue;

		strncat(cleaned, &orig[x], 1);
	}

	return cleaned;
}
#endif
