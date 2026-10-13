IMAGE_FSTYPES = "ubi"

UUU_TEMPLATE_NAME ??= "uuu-nand.auto.template"

# kernel-base recommends the versioned kernel-image-<ver> package
BAD_RECOMMENDATIONS:append = " kernel-image-*"
# NAND flash and UBI tools
IMAGE_INSTALL:append = " mtd-utils"

# UBI sizing
IMAGE_ROOTFS_ALIGNMENT = "128"
IMAGE_ROOTFS_EXTRA_SPACE = "${@2*128}"
IMAGE_OVERHEAD_FACTOR = "1"
IMAGE_ROOTFS_SIZE ?= "${@${ROOTFS_PARTITION_SIZE}-(${IMAGE_ROOTFS_EXTRA_SPACE})}"

MKUBIFS_LEB_SIZE = "126976"
MKUBIFS_LEB_CNT ?= "${@int(((${IMAGE_ROOTFS_SIZE} + ${IMAGE_ROOTFS_EXTRA_SPACE}) * 1024 + ${MKUBIFS_LEB_SIZE} - 1) / ${MKUBIFS_LEB_SIZE})}"
MKUBIFS_ARGS ?= "-F -x zlib -m 2048 -e ${MKUBIFS_LEB_SIZE} -c ${MKUBIFS_LEB_CNT}"
UBINIZE_ARGS ?= "-m 2048 -s 2048 -p 128KiB"
UBI_VOLNAME = "rootfs"
