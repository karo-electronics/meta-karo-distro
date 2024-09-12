SUMMARY = "Ka-Ro bootfs Image"
LICENSE = "MIT"

require karo-subimage.inc

IMAGE_PARTITION_MOUNTPOINT = "/boot"
IMAGE_PARTITION_MOUNTPOINT:stm32mpcommon = "${STM32MP_BOOTFS_MOUNTPOINT_IMAGE}"
IMAGE_NAME_SUFFIX = ".bootfs"

IMAGE_ROOTFS_MAXSIZE ?= "${@ d.getVar('BOOTFS_PARTITION_SIZE') if d.getVar('BOOTFS_PARTITION_SIZE') != None else ""}"

IMAGE_ROOTFS_SIZE = "${@ d.getVarFlags('IMAGE_ROOTFS_SIZE')['bootfs'] if 'bootfs' in d.getVarFlags('IMAGE_ROOTFS_SIZE') else d.getVar('BOOTFS_PARTITION_SIZE') if d.getVar('BOOTFS_PARTITION_SIZE') != None else 999999}"

IMAGE_INSTALL:append = " kernel-devicetree kernel-image"

# Define specific EXT4 command line:
#   - Create minimal inode number (as it is done by default in image_types.bbclass)
#   - Add label name (maximum length of the volume label is 16 bytes)
#   - Deactivate metadata_csum and dir_index (hashed b-trees): update not supported
#     by U-Boot; set inode size to 256 to support timestamps beyond 2038-01-19
EXTRA_IMAGECMD:ext4 += "-L ${SUBIMAGE_NAME} -O ^metadata_csum,^dir_index"

reformat_rootfs:append() {
    for f in "${IMAGE_ROOTFS}"/* ];do
        if [ -h "$f" ];then
            target="$(readlink "$f")"
            if [ -f "${IMAGE_ROOTFS}/$target" ];then
                bbnote "Replacing symlink '$f' with actual file '$target'"
                mv -v "${IMAGE_ROOTFS}/$target" "$f"
            else
                rm -v "$f"
            fi
        fi
    done
}
