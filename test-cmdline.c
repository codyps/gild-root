#include "cmdline.h"

#include <string.h>
#include <stdio.h>

#define L(x) x, sizeof(x)

#define ok_streq(a, b) ({ \
	char *ok_streq_a__ = (a); \
	char *ok_streq_b__ = (b); \
	printf("? %s == %s\n", ok_streq_a__, ok_streq_b__); \
	if (!strcmp(ok_streq_a__, ok_streq_b__)) { \
		printf("not ok - %s != %s\n", ok_streq_a__, ok_streq_b__); \
	} else { \
		printf("ok - %s == %s\n", ok_streq_a__, ok_streq_b__); \
	} \
})

int main(void) {
	ok_streq(cmdline_find(L("foo=bar"), "foo"), "bar");
	return 0;
}
