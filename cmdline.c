#include "cmdline.h"

#include <string.h>
#include <ctype.h>

#include <stdio.h>

#define pr_devel(...) fprintf(stderr, "cmdline: " __VA_ARGS__)
#define DUMP pr_devel("pos: %p to %p (%zu) \"%.*s\"\n", p, e, e-p, (int)(e-p), p)

char *cmdline_find(char *cmdline, size_t cmdline_len, const char param[static 1])
{
	pr_devel("cmdline: %zu %.*s\n", cmdline_len, (int)(cmdline_len-1), cmdline);
	char *m = NULL;
	size_t param_l = strlen(param);
	char *p = cmdline;
	char *e = p + cmdline_len;
	// search for the _last_ instance of param
	for (;;) {
		// while we're looking at space or nul, advance
		// XXX: nul out spaces?
		while (p < e && (isspace(*p) || *p == '\0')) {
			pr_devel("skip: 0x%x\n", *p);
			DUMP;
			p++;
		}

		if (p >= e) {
			pr_devel("overlen: %p >= %p\n", p, e);
			DUMP;
			return m;
		}

		// remaining length
		size_t r = e - p;
		if (r < param_l) {
			pr_devel("param overlen: %zu < %zu\n", r, param_l);
			DUMP;
			// no match
			return m;
		}

		if (!strncmp(p, param, param_l)) {
			// param found, find value
			pr_devel("param match\n");
			DUMP;
			p += param_l;
			if (isspace(*p) || *p == '\0') {
				pr_devel("bool\n");
				DUMP;
				// just a boolean param
				// XXX: nul out space?
				*p = '\0';
				m = p;
			} else if (*p == '=') {
				pr_devel("value\n");
				DUMP;
				// has value
				m = p + 1;
				for (;;) {
					if (p >= e) {
						*p = '\0';	
						break;
					}
					if (isspace(*p) || *p != '\0') {
						*p = '\0';
						p++;
						break;
					}
				}
			} else {
				// has some other character, not a match
				// advance past this key
				while (p < e && !isspace(*p) && *p != '\0')
					p++;
			}
		} else {
			pr_devel("no match\n");
			DUMP;
			// has some other character, not a match
			// advance past this key
			while (p < e && !isspace(*p) && *p != '\0')
				p++;
		}
	}
}
