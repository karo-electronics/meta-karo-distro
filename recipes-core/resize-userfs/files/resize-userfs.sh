#!/bin/sh
#
# Grow the userfs partition (/usr/local) and its ext4 filesystem into
# the free space left on the storage device. No-op once nothing is
# left, so a failed attempt is retried on the next boot.

# Whole-disk device behind the partition mounted at /usr/local, so the
# boot media (SD vs eMMC) need not be known in advance.
PART_DEV=$(basename "$(awk '$2 == "/usr/local" { print $1; exit }' /proc/mounts)")
[ -d "/sys/class/block/$PART_DEV" ] || exit 1
PART_NUM=$(cat "/sys/class/block/$PART_DEV/partition")
DEV=$(basename "$(dirname "$(readlink "/sys/class/block/$PART_DEV")")")

# Nothing to do if no unpartitioned space is left.
FREE_SECTORS=$(sfdisk -F "/dev/$DEV" 2>/dev/null | awk 'NR == 1 { print $(NF - 1) }')
[ "$FREE_SECTORS" -gt 0 ] 2>/dev/null || exit 0

# Grow the partition to the end of the device.
echo ",+" | sfdisk -q --no-reread --no-tell-kernel --force -N "$PART_NUM" "/dev/$DEV" 2>/dev/null || exit 1

# Notify the kernel to re-read the partition table.
partx --update "/dev/$PART_DEV" || exit 1

# Grow the ext4 filesystem online (mounted).
resize2fs "/dev/$PART_DEV" || exit 1
