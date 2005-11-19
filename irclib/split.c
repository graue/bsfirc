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
void 
i_free(split_t * s)
{
	size_t          x;

	for (x = 0; x < s->num; x++)
		free(s->data[x]);

	free(s->data);
	free(s);
}

/* PROTO */
split_t        *
i_split(char *str)
{
	size_t          x, y, len, cur, num = 0;
	split_t        *stmp;

	len = strlen(str);

	for (x = 0; x < len; x++)
		if (str[x] == ' ')
			num++;

	num++;
	stmp = malloc(sizeof(split_t));
	stmp->data = malloc(sizeof(uint8_t *) * num);
	stmp->num = num;

	for (x = 0, y = 0, cur = 0; x <= len; x++) {
		if (str[x] == ' ' || x == len) {
			stmp->data[cur] = malloc(x - y + 1);
			memcpy(stmp->data[cur], str + y, x - y);
			stmp->data[cur][x - y] = 0;
			y = x + 1;
			cur++;
		}
	}

	return stmp;
}
