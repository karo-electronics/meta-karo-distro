inherit core-image

require ${@ "recipes-core/images/st-machine-common-stm32mp.inc" if 'stm32mp1' in d.getVar('MACHINEOVERRIDES').split(":") else ""}

IMAGE_FSTYPES_append = " ext4"

SUBIMAGE_NAME ?= "${@ "${IMAGE_BASENAME}".split("-")[-1]}"

IMAGE_ROOTFS_SIZE ?= "32768"

IMAGE_PREPROCESS_COMMAND_append = "reformat_rootfs;"

# Cleanup rootfs newly created
reformat_rootfs() {
    if [ -z "${IMAGE_PARTITION_MOUNTPOINT}" ];then
        bbfatal "IMAGE_PARTITION_MOUNTPOINT is not set for '${SUBIMAGE_NAME}'"
    fi
    if [ -d ${IMAGE_ROOTFS}${IMAGE_PARTITION_MOUNTPOINT} ]; then
        rm -rvf "${WORKDIR}/${SUBIMAGE_NAME}"
        mv -v "${IMAGE_ROOTFS}${IMAGE_PARTITION_MOUNTPOINT}" "${WORKDIR}/${SUBIMAGE_NAME}"
        rm -rv "${IMAGE_ROOTFS}"
        mv -v "${WORKDIR}/${SUBIMAGE_NAME}" "${IMAGE_ROOTFS}"
        ls -alR "${IMAGE_ROOTFS}"
    else
        bbwarn "${IMAGE_PARTITION_MOUNTPOINT} folder not available in rootfs folder, no reformat done..."
    fi
}

# Define specific EXT4 command line:
#   - Create minimal inode number (as it is done by default in image_types.bbclass)
#   - Add label name (maximum length of the volume label is 16 bytes)
#   - Deactivate metadata_csum and dir_index (hashed b-trees): update not supported
#     by U-Boot; set inode size to 256 to support timestamps beyond 2038-01-19
EXTRA_IMAGECMD_ext4 += "-I 256 -L ${SUBIMAGE_NAME} -O ^metadata_csum,^dir_index"
