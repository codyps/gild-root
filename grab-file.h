#pragma once
#include <stddef.h>
#include <unistd.h>
ssize_t fd_read_to_end(int fd, char **buf, size_t *len);
