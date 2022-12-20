SUMMARY = "Ka-Ro userfs Image"
LICENSE = "MIT"

require karo-subimage.inc

IMAGE_FSTYPES = "ext4 tar.bz2"

IMAGE_PARTITION_MOUNTPOINT = "/usr/local"
IMAGE_PARTITION_MOUNTPOINT:stm32mp1 = "${STM32MP_USERFS_MOUNTPOINT_IMAGE}"
IMAGE_NAME_SUFFIX = ".userfs"

IMAGE_ROOTFS_SIZE ?= "32768"
IMAGE_ROOTFS_SIZE:stm32mp1 = "${USERFS_PARTITION_SIZE}"
IMAGE_ROOTFS_MAXSIZE:stm32mp1 = "${USERFS_PARTITION_SIZE}"

USERFS_PKGS ?= " \
        ${@bb.utils.contains('DISTRO_FEATURES','copro','packagegroup-copro','',d)} \
"

IMAGE_INSTALL:append = " \
        ${USERFS_PKGS} \
"
