#pragma once
#include <stddef.h>
/*
 * devpaths are strings that look like paths and describe a kobject chain in
 * the linux kernel. each element of the path is a kobject.
 *
 * devpaths allow exact description of devices, and are similar to open
 * firmware paths. they are useful for refering to block devices by their
 * physical location without any other userspace support.
 */

/**
 * devpath_from_sysfs_path - given a base location for sysfs and a path within sysfs, return the corresponding devpath
 */
ssize_t devpath_from_sysfs_path(int sysfs_dirfd, const char *restrict sysfs_path, char *restrict devpath_buf, size_t buf_size);

/**
 * blkdev_from_devpath - given a devpath of a block device, return the path to the block device file
 */
const char *blkdev_from_devpath(const char *restrict devpath);
