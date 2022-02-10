SUMMARY = "Ka-Ro bootfs Image"
LICENSE = "MIT"

require karo-subimage.inc

IMAGE_FSTYPES_append = " ext4"

IMAGE_PARTITION_MOUNTPOINT = "/boot"
IMAGE_PARTITION_MOUNTPOINT_stm32mp1 = "${STM32MP_BOOTFS_MOUNTPOINT_IMAGE}"
IMAGE_NAME_SUFFIX_stm32mp1 = ".${STM32MP_BOOTFS_LABEL}"

IMAGE_ROOTFS_SIZE ?= "32768"
IMAGE_ROOTFS_SIZE_stm32mp1 = "${BOOTFS_PARTITION_SIZE}"
IMAGE_ROOTFS_MAXSIZE_stm31mp1 = "${BOOTFS_PARTITION_SIZE}"

IMAGE_PREPROCESS_COMMAND_append = "reformat_rootfs;"

CORE_IMAGE_EXTRA_INSTALL += "kernel-devicetree kernel-image"
