#include "grab-file.h"

#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>

int main(int argc, char *argv[]) {
	int i;

	char *buf = NULL;
	size_t l;
	int e = 0;

	for (i = 1; i < argc; i++) {
		int fd = open(argv[i], O_RDONLY);
		if (fd < 0) {
			fprintf(stderr,
				"> could not open: %s\n"
				"> error: %s\n"
			, argv[i], strerror(errno));
			errno = 0;
			e++;
			continue;
		}

		ssize_t r = fd_read_to_end(fd, &buf, &l);
		close(fd);

		if (r < 0) {
			fprintf(stderr, "error reading %s: %s\n",
					argv[i], strerror(errno));
			e++;
		}

		fwrite(buf, l, 1, stdout);

		putchar('\n');
	}
	
	free(buf);

	return e > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
