#@DESCRIPTION: Common Machine configuration for STM32 systems

require st-machine-flashlayout-stm32mp.inc

# Define rootfs image
STM32MP_ROOTFS_IMAGE = "${IMAGE_BASENAME}-${MACHINE}.rootfs"
STM32MP_ROOTFS_FSTYPE = "ext4"

# Define boot image
STM32MP_BOOTFS_IMAGE = "${IMAGE_BASENAME}-${MACHINE}.boot"
STM32MP_BOOTFS_FSTYPE = "ext4"

# Define userfs image
STM32MP_USERFS_IMAGE = "${IMAGE_BASENAME}-${MACHINE}.userfs"
STM32MP_USERFS_FSTYPE = "ext4"

# Requires the seperate partition images in the flash package
KARO_PACKAGE_INCLUDE_PARTITIONS = "1"
