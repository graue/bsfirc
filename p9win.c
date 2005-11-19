#define _RESEARCH_SOURCE
#define _LOCK_EXTENSION
#define _QLOCK_EXTENSION
#define _BSD_EXTENSION
#define _POSIX_EXTENSION
#define _POSIX_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <u.h>
#include <fmt.h>
#include <lock.h>
#include <qlock.h>
#include <draw.h>

/* PROTO */
int
getwidth(void)
{
	int             linewidth, charwidth, fd, num;
	char            buf[128], *fn;
	Font           *font;

	if ((fn = getenv("font")) == NULL)
		return -1;
	if ((font = openfont(NULL, fn)) == NULL)
		return -1;
	if ((fd = open("/dev/window", O_RDONLY)) < 0) {
		font = NULL;
		return -1;
	}
	num = read(fd, buf, 60);
	close(fd);

	if (num < 60) {
		font = NULL;
		return -1;
	}
	buf[num] = 0;

	linewidth = atoi(buf + 36) - atoi(buf + 12) - 25;
	charwidth = stringwidth(font, "0");

	return linewidth / charwidth;
}
