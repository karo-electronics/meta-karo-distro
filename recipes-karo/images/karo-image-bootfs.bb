SUMMARY = "Ka-Ro bootfs Image"
LICENSE = "MIT"

require karo-subimage.inc

IMAGE_FSTYPES = "ext4 tar.bz2"

IMAGE_PARTITION_MOUNTPOINT = "/boot"
IMAGE_PARTITION_MOUNTPOINT:stm32mp1 = "${STM32MP_BOOTFS_MOUNTPOINT_IMAGE}"
IMAGE_NAME_SUFFIX = ".bootfs"

IMAGE_ROOTFS_SIZE ?= "32768"
IMAGE_ROOTFS_SIZE:stm32mp1 = "${BOOTFS_PARTITION_SIZE}"
IMAGE_ROOTFS_MAXSIZE_stm31mp1 = "${BOOTFS_PARTITION_SIZE}"

IMAGE_INSTALL:append = " kernel-devicetree kernel-image"

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
