#include "grab-file.h"

#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define INITIAL_BUF_SZ 256
#define MIN_BUF_SZ 256
#define BUF_GROW_MULT 1
#define BUF_GROW_ADD 1024*1024

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

char *grab_file(const char *path, size_t *size)
{
	int fd = open(path, O_RDONLY);
	char *buf = NULL;
	size_t s = 0;
	ssize_t r = fd_read_to_end(fd, &buf, &s);
	if (r < 0) {
		free(buf);
		*size = 0;
		return NULL;
	}

	// XXX: could use realloc to shrink the allocation
	*size = (size_t)r;
	return buf;
}
