SUMMARY = "Ka-Ro bootfs Image"
LICENSE = "MIT"

inherit core-image

IMAGE_FSTYPES_remove = "wic"

IMAGE_NAME_SUFFIX = ".${STM32MP_BOOTFS_LABEL}"

IMAGE_PARTITION_MOUNTPOINT = "${STM32MP_BOOTFS_MOUNTPOINT_IMAGE}"

# Set ROOTFS_MAXSIZE to expected ROOTFS_SIZE to use the whole disk partition and leave extra space to user
IMAGE_ROOTFS_SIZE        = "${BOOTFS_PARTITION_SIZE}"
IMAGE_ROOTFS_MAXSIZE     = "${BOOTFS_PARTITION_SIZE}"

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
        mv ${IMAGE_ROOTFS}${IMAGE_PARTITION_MOUNTPOINT}/* ${IMAGE_ROOTFS}/
        # Remove empty /boot folder
        rm -rf ${IMAGE_ROOTFS}${IMAGE_PARTITION_MOUNTPOINT}/
    else
        bbwarn "${IMAGE_PARTITION_MOUNTPOINT} folder not available in rootfs folder, no reformat done..."
    fi
}
