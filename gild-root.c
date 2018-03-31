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

#define STR_(x) #x
#define STR(x) STR_(x)


#define RW_BEFORE "/mnt"
#define OVERLAY_BEFORE "/tmp"

// based on openwrt
#define ROOT_AT_EXIT "/rom"
#define RW_AT_EXIT "/overlay"


// TODO: take this from cmdline, config, or use UUID to auto-select
#define RW_DEV "/dev/sda2"
// TODO: take this from cmdline, config, or autodetect
#define RW_DEV_FS "btrfs"
// TODO: take from cmdline/config
#define RW_DEV_FSOPTS ""
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
		goto exec_init;
	}

	// TODO: examine the cmdline

	// TODO: mount devtmpfs if not already mounted?
	//   - wouldn't want this if we wanted to use a static /dev from the RO root.
	//   - make conditional on config

	// TODO: run other prep steps (formatting?)

	r = mount_warn(RW_DEV, RW_BEFORE, RW_DEV_FS, RW_DEV_FLAGS, RW_DEV_FSOPTS); 
	if (r == -1) {
		// TODO: potentially use some fallback steps (formatting) &
		// retry the mount once.
		
		// fallback to non-overlay mount.
		pr("no overlay mounted, running init");
		goto exec_init;
	}

	// TODO: for generic hole punching create extra dirs
	// FIXME: should we select perms more carefully?
	mkdir_may_exist(RW_BEFORE "/upper", 0755);
	mkdir_may_exist(RW_BEFORE "/work",  0755);

	// TODO: generate a meaningful device name. Including the device name
	// of the lower would be good.
	r = mount_warn("gild:" RW_DEV, OVERLAY_BEFORE, "overlay", OVERLAY_FLAGS,
		"lowerdir=/,"
		"upperdir=" RW_BEFORE "/upper,"
		"workdir=" RW_BEFORE "/work"
	);
	if (r == -1) {
		pr("no overlay mounted, running init\n");
		goto exec_init;
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
		goto exec_init;
	}

	r = umount(ROOT_AT_EXIT "/proc");
	if (r == -1) {
		pr("warning: failed to unmount old proc at " ROOT_AT_EXIT "/proc : %s", strerror(errno));
	}

	mount_warn(ROOT_AT_EXIT "/dev", "/dev", NULL, MS_MOVE, "");

exec_init:
	argv[0] = (char *)NEXT_INIT;
	execv_(NEXT_INIT, argv);
	// if we get here, we've failed the init.
	// exiting will result in a panic.
	//
	// TODO: consider fallback to execing other inits, depending on the
	// configuration/cmdline/config file.
	return EXIT_FAILURE;
}
