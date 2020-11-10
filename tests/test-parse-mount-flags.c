#include "parse-mount-flags.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/mount.h>

static void t(const char *flag_str, unsigned long flag_int)
{
	unsigned long flag_out = 0;
	int r = parse_mount_flags(flag_str, &flag_out);
	if (r < 0) {
		fprintf(stderr, "ERR: %s => %d\n", flag_str, r);
		exit(EXIT_FAILURE);
	}

	if (flag_int != flag_out) {
		fprintf(stderr, "ERR: expect: %#04lx != got: %#04lx (%s)\n", flag_int, flag_out, flag_str);
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "OK: %s => %#04lx\n", flag_str, flag_out);
}

int main(void) {
	
	t("dirsync", MS_DIRSYNC);
	t("dirsync,lazytime", MS_DIRSYNC|MS_LAZYTIME);
	return 0;
}
