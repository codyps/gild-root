#include "syspath.h"

int devpath_from_sysfs_path(int sysfs_dirfd, const char *restrict sysfs_path, const char *restrict devpath_buf, size_t devpath_buf_len)
{
	// for each element of `path`, check if it has a `uevent` file.
	// `uevent` files only exist for kobjects. if no `uevent` is present,
	// we can exclude them from the syspath
	
	size_t sysfs_p_len = strlen(sysfs_path);

	// note: if `sysfs_path` were writable or if syscalls accepted
	// (ptr,len) file paths, we could be fancy here and avoid this vla.
	char iter_path[sysfs_p_len];
	size_t offs = 0;
	char *devpath_pos = devpath_buf;
	// NOTE: `-1` to preserve space for '\0'
	size_t devpath_rem = devpath_buf_len - 1;

	if (devpath_rem) {
		// XXX: consider if we want to start devpaths with `/`
		// XXX: consider if we should return ENOMEM when devpath_buf space runs out
		*devpath_pos = '/';
		devpath_pos++;
		devpath_rem--;
	}
	
	// build 2 simultanious paths:
	//  - the devpath
	//  - the path into sysfs representing the devpath as we iterate (this is an incrimental copy of `sysfs_path`
	for (;;) {
		// copy element in `path` into `devpath_buf`
		const char *pp = sysfs_path + offs;
		if (*pp == '\0') {
			// we've consumed the entire path and should have a compelte buffer
			break;
		}

		char *ne = strchr(pp, '/');
		size_t ne_ffs;
		if (!ne) {
			// we're at the last element
			// do something
			ne_offs = strlen(pp);
		} else {
			ne_offs = ne - pp;
		}
		
		memcpy(iter_path + offs, sysfs_path + offs, ne_offs);
		iter_path[offs + ne_offs] = '\0';

		// check for element existence (return error if refering to a non-existent element)
		struct stat sbuf;
		int r = fstatat(sysfs_dirfd, iter_path, &sbuf, 0);
		if (r < 0) {
			// this element of the input path doesn't exist
			return -ENOENT;
		}

		// append `uevent`
		memcpy(iter_path + offs + ne_offs, "uevent", sizeof("uevent"));

		// check for existence
		r = fstatat(sysfs_dirfd, iter_path, &sbuf, 0);
		if (r < 0) {
			// XXX: examine error code, restrict to `uevent` not existing
			continue;
		}

	 	// if exists, copy element into devpath
		if (devpath_rem < ne_offs) {
			// no more space, fuse
			*devpath_pos = '\0';
			devpath_rem = 0;
		}

		memcpy(devpath_pos, iter_path, ne_offs);
		devpath_pos += ne_offs;
		devpath_rem -= ne_offs;

		// and advance append location to before `uevent`
		offs += ne_offs;
	}

	return -1;
}

char *blkdev_from_devpath(const char *restrict devpath)
{
	// for each in /sys/class/block
	// use readlink, and resolve path to a sysfs path
	// match resolved path against our devpath input
	// fnmatch?
}
