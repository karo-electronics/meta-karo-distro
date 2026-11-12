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

# The separate partition images exported from the wic partition
# generation are used for the STM32 flashlayout generation.
STM32MP_PARTITIONS_LIST  = "${STM32MP_BOOTFS_IMAGE}.${STM32MP_ROOTFS_FSTYPE}"
STM32MP_PARTITIONS_LIST += "${STM32MP_ROOTFS_IMAGE}.${STM32MP_BOOTFS_FSTYPE}"
STM32MP_PARTITIONS_LIST += "${STM32MP_USERFS_IMAGE}.${STM32MP_USERFS_FSTYPE}"

# Copy the partition images into the flash package directory.
python do_create_flash_package:append() {
    partitions = d.getVar('STM32MP_PARTITIONS_LIST').split()
    for partition in partitions:
        do_copyfile("%s" % os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), partition), d.getVar('KARO_PACKAGE_DIR'))
}
