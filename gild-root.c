#include <stdio.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/syscall.h>

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

static
int pivot_root(const char new_root[static 1], const char put_old[static 1])
{
	return syscall(SYS_pivot_root, new_root, put_old);
}

int main(int argc, char **argv)
{
	(void)argc;

	puts("gild-root: " STR(GILD_ROOT_VERSION));

	mount("proc", "/proc", "proc", MS_REMOUNT, "");
	mount(RW_DEV, RW_BEFORE, RW_DEV_FS, RW_DEV_FLAGS, RW_DEV_FSOPTS); 

	// TODO: for generic hole punching create extra dirs
	// FIXME: should we select perms more carefully?
	mkdir(RW_BEFORE "/upper", 0755);
	mkdir(RW_BEFORE "/work",  0755);

	// TODO: generate a meaningful device name. Including the device name
	// of the lower would be good.
	mount("gild:" RW_DEV, OVERLAY_BEFORE, "overlay", OVERLAY_FLAGS,
		"lowerdir=/,"
		"upperdir=" RW_BEFORE "/upper,"
		"workdir=" RW_BEFORE "/work"
	);

	// address sanitizer uses /proc, lets make sure it's always mounted at /proc to avoid any issues.
	mount("proc", "/proc" OVERLAY_BEFORE, "proc", 0, "");

	// shouldn't be necessary to create these if the lower is correctly created, but lets avoid any supprises
	// TODO: warn if lower doesn't have these
	mkdir(OVERLAY_BEFORE ROOT_AT_EXIT, 0755);
	mkdir(OVERLAY_BEFORE RW_AT_EXIT, 0755);

	// for dev mount that we _might_ move
	// TODO: probe existing mounts in `/` before creating this & handle
	// all of them.
	mkdir(OVERLAY_BEFORE "/dev", 0755);

	pivot_root(OVERLAY_BEFORE, OVERLAY_BEFORE ROOT_AT_EXIT);

	umount(ROOT_AT_EXIT "/proc");

	mount(ROOT_AT_EXIT "/dev", "/dev", NULL, MS_MOVE, "");

	argv[0] = NEXT_INIT;
	execv(NEXT_INIT, argv);
	return 0;
}
