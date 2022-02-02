inherit core-image

IMAGE_FSTYPES_append = " ext4"

SUBIMAGE_NAME ?= "${@ "${IMAGE_BASENAME}".split("-")[-1]}"

IMAGE_ROOTFS_SIZE ?= "32768"

IMAGE_PREPROCESS_COMMAND_append = "reformat_rootfs;"

# Cleanup rootfs newly created
reformat_rootfs() {
    set -x
    if [ -d ${IMAGE_ROOTFS}/${IMAGE_PARTITION_MOUNTPOINT} ]; then
#        find "${IMAGE_ROOTFS}"/* -depth \( -name "${IMAGE_PARTITION_MOUNTPOINT}" -prune \) -o -exec rm -rv \{\} \;
#        find "${IMAGE_ROOTFS}/${IMAGE_PARTITION_MOUNTPOINT}" -depth -exec mv -v \{\} "${${WORKDIR}/${SUBIMAGE_NAME}" \;
        mv -vi "${IMAGE_ROOTFS}/${IMAGE_PARTITION_MOUNTPOINT}" "${WORKDIR}/${SUBIMAGE_NAME}"
        rm -rv "${IMAGE_ROOTFS}"
        mv -vi "${WORKDIR}/${SUBIMAGE_NAME}" "${IMAGE_ROOTFS}"
        ls -alR "${IMAGE_ROOTFS}"
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
EXTRA_IMAGECMD_ext4 = "-i 4096 -L ${SUBIMAGE_NAME} -O ^metadata_csum,^dir_index"
