#pragma once
#include <stddef.h>

struct cmdline {
	char *b;
	size_t l;
};

// If the param is not found, returns NULL
// If the param is found & has a value, returns a pointer to the start of that value.
// If there is no value, returns a pointer to '\0'.
char *cmdline_find(char *cmdline, size_t cmdline_len, const char param[static 1]);
