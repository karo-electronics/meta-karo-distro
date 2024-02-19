IMAGE_LINGUAS = ""

IMAGE_FEATURES:remove = "tools-debug"

# remove packages that pull in openssl
IMAGE_INSTALL:remove = "curl git"

ROOTFS_PARTITION_SIZE ?= "${@bb.utils.contains('MACHINE_FEATURES',"nand", \
                          64*1024,128*1024,d)}"

# align partitions to eraseblock size
IMAGE_ROOTFS_ALIGNMENT = "128"

UBI_VOLNAME = "rootfs"

export UBIVOL_FILES_rootfs = "${IMAGE_ROOTFS}"
