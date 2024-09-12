SUMMARY = "Ka-Ro userfs Image"
LICENSE = "MIT"

require karo-subimage.inc

IMAGE_PARTITION_MOUNTPOINT = "/usr/local"
IMAGE_PARTITION_MOUNTPOINT:stm32mpcommon = "${STM32MP_USERFS_MOUNTPOINT_IMAGE}"
IMAGE_NAME_SUFFIX = ".userfs"

IMAGE_ROOTFS_MAXSIZE ?= "${@ d.getVar('USERFS_PARTITION_SIZE') if d.getVar('USERFS_PARTITION_SIZE') != None else "32768"}"
IMAGE_ROOTFS_SIZE:stm32mpcommon = "${USERFS_PARTITION_SIZE}"
IMAGE_ROOTFS_MAXSIZE:stm32mpcommon = "${USERFS_PARTITION_SIZE}"

USERFS_PKGS ?= " \
        ${@bb.utils.contains('DISTRO_FEATURES','copro','packagegroup-copro','',d)} \
"

IMAGE_INSTALL:append = " \
        ${USERFS_PKGS} \
"
