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

STM32MP_PARTITIONS_LIST  = "${STM32MP_BOOTFS_IMAGE}.${STM32MP_ROOTFS_FSTYPE}"
STM32MP_PARTITIONS_LIST += "${STM32MP_ROOTFS_IMAGE}.${STM32MP_BOOTFS_FSTYPE}"
STM32MP_PARTITIONS_LIST += "${STM32MP_USERFS_IMAGE}.${STM32MP_USERFS_FSTYPE}"

python do_create_install_scripts:append() {
    partitions = d.getVar('STM32MP_PARTITIONS_LIST').split()
    for partition in partitions:
        do_copyfile("%s" % os.path.join(img_deploy_dir, partition), img_dirname)
}
