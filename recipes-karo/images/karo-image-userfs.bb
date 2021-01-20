SUMMARY = "Ka-Ro userfs Image"
LICENSE = "MIT"

inherit core-image

IMAGE_FSTYPES_remove = "wic"

IMAGE_NAME_SUFFIX = ".${STM32MP_USERFS_LABEL}"

IMAGE_PARTITION_MOUNTPOINT = "${STM32MP_USERFS_MOUNTPOINT_IMAGE}"

IMAGE_ROOTFS_MAXSIZE = "${USERFS_PARTITION_SIZE}"
IMAGE_ROOTFS_SIZE = "${USERFS_PARTITION_SIZE}"

IMAGE_PREPROCESS_COMMAND_append = "reformat_rootfs;"

# Cleanup rootfs newly created
reformat_rootfs() {
    if [ -d ${IMAGE_ROOTFS}${IMAGE_PARTITION_MOUNTPOINT} ]; then
        if [ -z $(ls -A ${IMAGE_ROOTFS}${IMAGE_PARTITION_MOUNTPOINT}/) ]; then
	        rm -rf ${IMAGE_ROOTFS}/*
	    else
            # Keep only IMAGE_PARTITION_MOUNTPOINT folder
            for f in $(ls -1 -d ${IMAGE_ROOTFS}/*/*/ | grep -v ${IMAGE_PARTITION_MOUNTPOINT}/)
            do
                rm -rf $f
            done

            for f in $(ls -1 -d ${IMAGE_ROOTFS}/*/ | grep -v $(dirname ${IMAGE_PARTITION_MOUNTPOINT}/))
            do
                rm -rf $f
            done

            # Move all expected files in /rootfs
            mv ${IMAGE_ROOTFS}${IMAGE_PARTITION_MOUNTPOINT}/* ${IMAGE_ROOTFS}/
            # Remove empty folder
            rm -rf ${IMAGE_ROOTFS}${IMAGE_PARTITION_MOUNTPOINT}/ ${IMAGE_ROOTFS}$(dirname ${IMAGE_PARTITION_MOUNTPOINT}/)
	    fi
    else
        bbwarn "${IMAGE_PARTITION_MOUNTPOINT} folder not available in rootfs folder, no reformat done..."
    fi
}
