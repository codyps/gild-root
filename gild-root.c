#include <stdio.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <generated/gr/version.h>
#include "grab-file.h"
#include "cmdline.h"

#define STR_(x) #x
#define STR(x) STR_(x)


#define RW_BEFORE "/mnt"
#define OVERLAY_BEFORE "/tmp"

// based on openwrt
#define ROOT_AT_EXIT "/rom"
#define RW_AT_EXIT "/overlay"


// TODO: take from cmdline/config
#define RW_DEV_FLAGS MS_NOATIME
// TODO: take from cmdline/config
#define OVERLAY_FLAGS MS_NOATIME
// TODO: take from cmdline/config
#define NEXT_INIT "/sbin/init"

#define pr_fmt "gild-root: "
#define pr(...) printf(pr_fmt __VA_ARGS__)

static
int pivot_root(const char new_root[static 1], const char put_old[static 1])
{
	return syscall(SYS_pivot_root, new_root, put_old);
}

static
int mount_warn(const char *dev, const char *mount_point, const char *fs_type, int opts, const char *flags)
{
	int r = mount(dev, mount_point, fs_type, opts, flags);
	if (r == -1) {
		pr("failed mount(\"%s\", \"%s\", \"%s\", %#x, \"%s\") = %d = %s\n",
				dev, mount_point, fs_type, opts, flags, errno, strerror(errno));
	}
	return r;
}

static
int execv_(const char *prgm, char **argv)
{
	int r = execv(prgm, argv);
	if (r == -1) {
		pr("failed to execute \"%s\": %d %s\n",
				prgm, errno, strerror(errno));
	}
	return r;
}

static
int mkdir_may_exist(const char *path, mode_t mode)
{
	int r = mkdir(path, mode);
	if (r == -1) {
		if (errno == EEXIST) {
			// may be a file or a directory, need to ask.
			struct stat buf;
			r = stat(path, &buf);
			if (r == -1) {
				pr("could not mkdir(\"%s\"): "
					"path exists, but could not determine if it is a dir: %s\n",
						path, strerror(errno));
				return r;
			}

			if (!S_ISDIR(buf.st_mode)) {
				pr("could not mkdir(\"%s\"): "
					"path exists and is not a directory\n",
					path);
				return -ENOTDIR;
			}
		} else {
			pr("could not mkdir(\"%s\"): %s\n",
					path, strerror(errno));
		}
	}

	return r;
}

__attribute__((__noreturn__))
static void
exec_init(char **argv)
{
	argv[0] = (char *)NEXT_INIT;
	execv_(NEXT_INIT, argv);
	// if we get here, we've failed the init.
	// exiting will result in a panic.
	//
	// TODO: consider fallback to execing other inits, depending on the
	// configuration/cmdline/config file.
	abort();
}

int main(int argc, char **argv)
{
	(void)argc;
	int r;

	puts("gild-root: " STR(GILD_ROOT_VERSION));

	// we _may_ be running after something that has already mounted proc, so we allow remount here.
	// XXX: if not already mounted, we only need this to read the cmdline.
	// Consider allowing an option to not read cmdline.
	//
	r = mount_warn("proc", "/proc", "proc", MS_REMOUNT, "");
	if (r == -1) {
		pr("no overlay mounted, running init");
		exec_init(argv);
	}

	size_t cmdline_size;
	char *cmdline;
	cmdline = grab_file("/proc/cmdline", &cmdline_size);

	const char *rw_device = cmdline_find(cmdline, cmdline_size, "gr.rw_device");
	if (!rw_device) {
		pr("no rw_device provided, running init");
		exec_init(argv);
	}

	const char *fs_type = cmdline_find(cmdline, cmdline_size, "gr.fs_type");
	if (!fs_type) {
		pr("no fs_type provided, running init");
		exec_init(argv);
	}

	const char *fs_opts = cmdline_find(cmdline, cmdline_size, "gr.fs_opts");
	if (!fs_opts)
		fs_opts = "";

	// TODO: mount devtmpfs if not already mounted?
	//   - wouldn't want this if we wanted to use a static /dev from the RO root.
	//   - make conditional on config

	// TODO: run other prep steps (formatting?)

	r = mount_warn(rw_device, RW_BEFORE, fs_type, RW_DEV_FLAGS, fs_opts); 
	if (r == -1) {
		// TODO: potentially use some fallback steps (formatting) &
		// retry the mount once.
		
		// fallback to non-overlay mount.
		pr("no overlay mounted, running init");
		exec_init(argv);
	}

	// TODO: for generic hole punching create extra dirs
	// FIXME: should we select perms more carefully?
	mkdir_may_exist(RW_BEFORE "/upper", 0755);
	mkdir_may_exist(RW_BEFORE "/work",  0755);

	// TODO: generate a more meaningful device name. Including the device
	// name of the lower would be good.
	const char *base = "gild:";
	char n[strlen("gild:") + strlen(rw_device) + 1];
	sprintf(n, "%s%s", base, rw_device);

	r = mount_warn(n, OVERLAY_BEFORE, "overlay", OVERLAY_FLAGS,
		"lowerdir=/,"
		"upperdir=" RW_BEFORE "/upper,"
		"workdir=" RW_BEFORE "/work"
	);
	if (r == -1) {
		pr("no overlay mounted, running init\n");
		exec_init(argv);
	}

	// address sanitizer uses /proc, lets make sure it's always mounted at /proc to avoid any issues.
	// NOTE: only warn & don't fail as we might still complete if this fails
	mount_warn("proc", "/proc" OVERLAY_BEFORE, "proc", 0, "");

	// shouldn't be necessary to create these if the lower is correctly created, but lets avoid any supprises
	mkdir_may_exist(OVERLAY_BEFORE ROOT_AT_EXIT, 0755);
	mkdir_may_exist(OVERLAY_BEFORE RW_AT_EXIT, 0755);

	// for dev mount that we _might_ move
	// TODO: probe existing mounts in `/` before creating this & handle
	// all of them.
	mkdir_may_exist(OVERLAY_BEFORE "/dev", 0755);

	r = pivot_root(OVERLAY_BEFORE, OVERLAY_BEFORE ROOT_AT_EXIT);
	if (r == -1) {
		pr("pivot failed: %s\n", strerror(errno));
		// TODO: tear down setup and exec init.
		exec_init(argv);
	}

	r = umount(ROOT_AT_EXIT "/proc");
	if (r == -1) {
		pr("warning: failed to unmount old proc at " ROOT_AT_EXIT "/proc : %s", strerror(errno));
	}

	// TODO: check if mounted so we can avoid warning on move failures due
	// to a lack of a devtmpfs
	mount_warn(ROOT_AT_EXIT "/dev", "/dev", NULL, MS_MOVE, "");

	exec_init(argv);
}
