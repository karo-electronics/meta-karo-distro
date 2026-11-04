#!/bin/sh
#
# Grow the userfs partition (/usr/local) and its ext4 filesystem into
# the free space remaining on the storage device. Runs at every boot,
# but is a no-op once the partition table has no free areas left, so a
# failed attempt is retried on the next boot.
#
# Only installed on wic (GPT) images: UBI images auto-resize the
# userfs volume at attach time (vol_flags=autoresize), and RAUC A/B
# layouts have no free space behind the userfs partition.

# Locate the storage device through the partition actually mounted at
# /usr/local, so the boot media (e.g. SD card instead of eMMC) does not
# have to be known in advance. '/sys/class/block/<part>' is a symlink
# ending in '<disk>/<part>' ('mmcblk0/mmcblk0p3'), so its parent
# directory names the whole-disk device.
PART_DEV=$(basename "$(awk '$2 == "/usr/local" { print $1; exit }' /proc/mounts)")
[ -d "/sys/class/block/$PART_DEV" ] || exit 1

DEV=$(basename "$(dirname "$(readlink "/sys/class/block/$PART_DEV")")")
PART_NUM=$(cat "/sys/class/block/$PART_DEV/partition")

# Nothing to do if the partition table is already full: the partition
# was grown on an earlier boot. The check reads the partition table
# itself, so it needs no writable filesystem and works backwards from
# the point where the resize last succeeded.
sfdisk -F "/dev/$DEV" | grep -q . || exit 0

# Grow the partition entry (sfdisk also relocates the backup GPT to
# the end of the device).
echo ",+" | sfdisk --no-reread --force -N "$PART_NUM" "/dev/$DEV" || exit 1

# Tell the kernel about the new partition size right away. With
# --no-reread sfdisk only writes the table to disk; until the kernel
# re-reads it (at the next reboot) /sys/class/block/<part>/size and
# with it resize2fs still report the old size. partx uses the BLKPG
# resize ioctl, which the kernel also allows on an in-use partition
# (unlike BLKRRPART, which fails with EBUSY while any partition of the
# disk is mounted).
partx --update "/dev/$DEV" || exit 1

# Grow the ext4 filesystem online (mounted).
resize2fs "/dev/$PART_DEV" || exit 1
