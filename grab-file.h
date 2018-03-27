#pragma once
#include <stddef.h>
#include <unistd.h>

char *grab_file(const char *path, size_t *size);

ssize_t fd_read_to_end(int fd, char **buf, size_t *len);
