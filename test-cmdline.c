#include "cmdline.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define L(x) x, sizeof(x)

static unsigned err_ct = 0;

#define ok_streq(a, b) ok_streq_(a, #a, b, #b)
static void ok_streq_(const char *a, const char *as, const char *b, const char *bs)
{
	printf("? %s == %s\n", a, b);
	printf("  %s == %s\n", as, bs);
	fflush(stdout);
	if (((a == NULL) ^ (b == NULL)) || strcmp(a, b)) {
		printf("not ok - %s != %s\n", a, b);
		err_ct ++;
	} else {
		printf("ok - %s == %s\n", a, b);
	}
}

int main(void) {
	char a[] = "foo=bar";
	ok_streq(cmdline_find(L(a), "foo"), "bar");
	return err_ct ? EXIT_FAILURE : EXIT_SUCCESS;
}
