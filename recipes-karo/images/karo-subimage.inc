inherit core-image

require ${@ "recipes-core/images/st-machine-common-stm32mp.inc" if 'stm32mpcommon' in d.getVar('MACHINEOVERRIDES').split(":") else ""}

IMAGE_FSTYPES:append = " ext4"

SUBIMAGE_NAME ?= "${@ "${IMAGE_BASENAME}".split("-")[-1]}"

IMAGE_ROOTFS_SIZE ?= "32768"

IMAGE_PREPROCESS_COMMAND:append = "reformat_rootfs;"

# Cleanup rootfs newly created
reformat_rootfs() {
    if [ -z "${IMAGE_PARTITION_MOUNTPOINT}" ];then
        bbfatal "IMAGE_PARTITION_MOUNTPOINT is not set for '${SUBIMAGE_NAME}'"
    fi
    rm -rf "${WORKDIR}/${SUBIMAGE_NAME}"
    if [ -d ${IMAGE_ROOTFS}${IMAGE_PARTITION_MOUNTPOINT} ]; then
        mv "${IMAGE_ROOTFS}${IMAGE_PARTITION_MOUNTPOINT}" "${WORKDIR}/${SUBIMAGE_NAME}"
        rm -r "${IMAGE_ROOTFS}"
        mv "${WORKDIR}/${SUBIMAGE_NAME}" "${IMAGE_ROOTFS}"
    else
        rm -rf "${IMAGE_ROOTFS}"/*
    fi
}
