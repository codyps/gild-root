#include "grab-file.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define INITIAL_BUF_SZ 256
#define MIN_BUF_SZ 256
#define BUF_GROW_MULT 2
#define BUF_GROW_ADD 0

ssize_t fd_read_to_end(int fd, char **buf, size_t *len)
{
	char *b = *buf;
	size_t l = *len;
	size_t p = 0;

	if (!b) {
		l = INITIAL_BUF_SZ;
		b = malloc(l);
		if (!b) {
			return -ENOMEM;
		}
	}

	for (;;) {
		ssize_t r = read(fd, b + p, l - p);
		if (r <= 0) {
			*buf = b;	
			*len = p;
			return r;
		}

		p += r;

		if ((l - p) < MIN_BUF_SZ) {
			size_t e = (BUF_GROW_MULT - 1) * l + BUF_GROW_ADD;
			size_t nl = l + e;
			char *nb = realloc(b, nl);
			if (!nb) {
				*buf = b;
				*len = p;
				return -ENOMEM;
			}

			l = nl;
			b = nb;
		}
	}
}
