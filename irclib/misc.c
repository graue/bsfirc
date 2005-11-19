/**  _        _ _ _
 ** (_)_ _ __| (_) |__
 ** | | '_/ _| | | '_ \
 ** |_|_| \__|_|_|_.__/
 **
 ** A simple library for creating IRC clients.
 **
 ** (C) 2005 by Claudio Leite
 **
 ** Please see the COPYING file for more details.
 **/

#include "irclib.h"

/* PROTO */
int
getbyteorder(void)
{
	uint16_t        blah = 0x5533;
	uint8_t         tmp;

	memcpy(&tmp, &blah, 1);

	if (tmp == 0x55)
		return HOST_BIG_ENDIAN;
	else
		return HOST_LITTLE_ENDIAN;
}

/* PROTO */
char *
tolower_str(const char *orig)
{
	char *ns;
	int len = strlen(orig);
	size_t x;

	ns = malloc(len+1);
	for(x = 0; x < len; x++)
		ns[x] = tolower(orig[x]);

	ns[len] = 0;
	return ns;
}
