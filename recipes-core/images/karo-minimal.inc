IMAGE_FSTYPES_append = "${@bb.utils.contains('MACHINE_FEATURES',"nand"," ubi","",d)}"

ROOTFS_PARTITION_SIZE ?= "${@bb.utils.contains('MACHINE_FEATURES',"nand", \
                          64*1024,128*1024,d)}"

# align partitions to eraseblock size
IMAGE_ROOTFS_ALIGNMENT = "128"

MKUBIFS_ARGS ?= "-F -x zlib -m 2048 -e ${MKUBIFS_LEB_SIZE} -c ${MKUBIFS_LEB_CNT}"
UBI_VOLNAME = "rootfs"

export UBIVOL_FILES_rootfs = "${IMAGE_ROOTFS}"
