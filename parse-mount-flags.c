#define _GNU_SOURCE
#include "parse-mount-flags.h"

#include <ccan/mem/mem.h>
#include <sys/mount.h>
#include <string.h>
#include <stdio.h>

int parse_mount_flags(const char *flag_str, unsigned long *flags)
{
	*flags = 0;
	// comma seperated strings
	for (;;) {
		if (*flag_str == '\0')
			return 0;

		char *p = strchrnul(flag_str, ',');
		size_t l = p - flag_str;

		if (0) {}
#define MS(flag, string) \
	else if (memeq(flag_str, l, string, sizeof(string) - 1)) { \
		*flags |= flag; \
	}
#include "mount-flags.def"
#undef MS
		else {
			fprintf(stderr, "ERR: unknown flag: %*s\n", (int)l, flag_str);
			return -1;
		}

		flag_str = p;
		if (*flag_str == ',')
			flag_str++;
	}
}
