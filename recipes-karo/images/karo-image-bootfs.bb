SUMMARY = "Ka-Ro bootfs Image"
LICENSE = "MIT"

inherit core-image

IMAGE_FSTYPES_append = " ext4"

IMAGE_PARTITION_MOUNTPOINT ?= "/boot"

IMAGE_NAME_SUFFIX = ".${STM32MP_BOOTFS_LABEL}"

IMAGE_PARTITION_MOUNTPOINT = "${STM32MP_BOOTFS_MOUNTPOINT_IMAGE}"

IMAGE_ROOTFS_SIZE_stm32mp1        = "${BOOTFS_PARTITION_SIZE}"
IMAGE_ROOTFS_MAXSIZE_stm31mp1     = "${BOOTFS_PARTITION_SIZE}"
IMAGE_ROOTFS_SIZE        ?= "32768"

IMAGE_PREPROCESS_COMMAND_append = "reformat_rootfs;"

CORE_IMAGE_EXTRA_INSTALL += "kernel-devicetree"

# Cleanup rootfs newly created
reformat_rootfs() {
    if [ -d ${IMAGE_ROOTFS}${IMAGE_PARTITION_MOUNTPOINT} ]; then
        # Keep only IMAGE_PARTITION_MOUNTPOINT folder
        for f in $(ls -d ${IMAGE_ROOTFS}/*/ | grep -v ${IMAGE_PARTITION_MOUNTPOINT}/)
        do
            rm -rf $f
        done

        # Move all expected files from /boot to /
        cp -Hv ${IMAGE_ROOTFS}${IMAGE_PARTITION_MOUNTPOINT}/Image ${IMAGE_ROOTFS}/
        cp -v ${IMAGE_ROOTFS}${IMAGE_PARTITION_MOUNTPOINT}/*.dtb ${IMAGE_ROOTFS}/
        # Remove empty /boot folder
        rm -rf ${IMAGE_ROOTFS}${IMAGE_PARTITION_MOUNTPOINT}/
    else
        bbwarn "${IMAGE_PARTITION_MOUNTPOINT} folder not available in rootfs folder, no reformat done..."
    fi
}

# Define specific EXT4 command line:
#   - Create minimal inode number (as it is done by default in image_types.bbclass)
#   - Add label name (maximum length of the volume label is 16 bytes)
#     So use IMAGE_NAME_SUFFIX name by removing the '.' and truncating to 16 caracters
#   - Deactivate metadata_csum and dir_index (hashed b-trees): update not supported
#     by U-Boot
EXTRA_IMAGECMD_ext4 = "-i 4096 -L bootfs -O ^metadata_csum,^dir_index"
