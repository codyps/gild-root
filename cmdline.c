#include "cmdline.h"

#include <string.h>
#include <ctype.h>

char *cmdline_find(char *cmdline, size_t cmdline_len, char param[static 1])
{
	char *m = NULL;
	size_t param_l = strlen(param);
	char *p = cmdline;
	char *e = p + cmdline_len;
	// search for the _last_ instance of param
	for (;;) {
		// while we're looking at space or nul, advance
		while (p < e && (isspace(*p) || *p == '\0'))
			p++;

		if (p >= e)
			return m;

		size_t r = e - p;
		if (r < param_l) {
			// no match
			return m;
		}

		if (!strncmp(p, param, r)) {
			p += param_l;	
			if (isspace(*p) || *p == '\0') {
				// just a boolean param
				m = p;
			} else if (*p == '=') {
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
			// has some other character, not a match
			// advance past this key
			while (p < e && !isspace(*p) && *p != '\0')
				p++;
		}
	}
}
