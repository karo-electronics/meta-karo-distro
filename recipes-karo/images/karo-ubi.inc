SRC_URI:append = " file://ubinize-karo.cfg.in"

# NAND flash and UBI tools
IMAGE_INSTALL:append = " mtd-utils"

# Multi-volume UBI image: the .ubi image carries three volumes:
#   boot   (vol_id 0): kernel image and DTB files
#   rootfs (vol_id 1): the root filesystem
#   userfs (vol_id 2): volume mounted at /usr/local, populated from the
#   rootfs /usr/local directory, autoresized into the remaining flash

BOOTFS_PARTITION_SIZE ?= "16384"
ROOTFS_PARTITION_SIZE ?= "32768"
USERFS_PARTITION_SIZE ?= "32768"
MKUBIFS_LEB_SIZE = "126976"

MKUBIFS_ROOTFS_LEB_CNT ?= "${@int((${ROOTFS_PARTITION_SIZE} * 1024 + ${MKUBIFS_LEB_SIZE} - 1) / ${MKUBIFS_LEB_SIZE})}"
MKUBIFS_ARGS_ROOTFS ?= "-x zlib -m 2048 -e ${MKUBIFS_LEB_SIZE} -c ${MKUBIFS_ROOTFS_LEB_CNT}"

MKUBIFS_BOOT_LEB_CNT ?= "${@int((${BOOTFS_PARTITION_SIZE} * 1024 + ${MKUBIFS_LEB_SIZE} - 1) / ${MKUBIFS_LEB_SIZE})}"
MKUBIFS_ARGS_BOOT ?= "-x zlib -m 2048 -e ${MKUBIFS_LEB_SIZE} -c ${MKUBIFS_BOOT_LEB_CNT}"

MKUBIFS_USERFS_LEB_CNT ?= "${@int((${USERFS_PARTITION_SIZE} * 1024 + ${MKUBIFS_LEB_SIZE} - 1) / ${MKUBIFS_LEB_SIZE})}"
MKUBIFS_ARGS_USERFS ?= "-x zlib -m 2048 -e ${MKUBIFS_LEB_SIZE} -c ${MKUBIFS_USERFS_LEB_CNT}"

UBINIZE_ARGS ?= "-m 2048 -s 2048 -p 128KiB"

# The boot volume is built from the kernel and bootloader deploy
# artifacts, not from the rootfs (BAD_RECOMMENDATIONS above keeps the
# kernel out of the rootfs).
do_image_ubi[depends] += "virtual/kernel:do_deploy virtual/bootloader:do_deploy"

# ubinize volume layout template
KARO_UBINIZE_CFG ?= "ubinize-karo.cfg.in"

karo_write_ubi_config() {
    sed -e "s|@WORKDIR@|${WORKDIR}|g" "${WORKDIR}/${KARO_UBINIZE_CFG}" > ${WORKDIR}/ubinize-${IMAGE_NAME}.cfg
}

karo_ubi_boot_volume() {
    rm -rf "${WORKDIR}/bootfs"
    install -d "${WORKDIR}/bootfs"
    install -m 0644 "${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}" \
        "${WORKDIR}/bootfs/${KERNEL_IMAGETYPE}"
    # Kernel DTBs and overlays
    for dtb in ${IMAGE_BOOT_FILES}; do
        install -m 0644 "${DEPLOY_DIR_IMAGE}/$dtb" "${WORKDIR}/bootfs/"
    done
}

karo_ubi_userfs_volume() {
    rm -rf "${WORKDIR}/userfs"
    install -d "${WORKDIR}/userfs"
    # Move the rootfs /usr/local content into the userfs volume; the
    # rootfs itself keeps an empty /usr/local as the mount point
    if [ -d "${IMAGE_ROOTFS}/usr/local" ]; then
        find "${IMAGE_ROOTFS}/usr/local" -mindepth 1 -maxdepth 1 \
            -exec mv -t "${WORKDIR}/userfs" {} +
    fi
}

addhandler karo_set_ubi_cmd
karo_set_ubi_cmd[eventmask] = "bb.event.RecipePostKeyExpansion"
python karo_set_ubi_cmd() {
    d.setVar('IMAGE_CMD:ubi', d.getVar('KARO_UBI_CMD', False))
    # KARO_UBI_CMD builds the rootfs volume itself.
    # Drop the stock ubifs image type dependency.
    d.setVar('IMAGE_TYPEDEP:ubi', '')
}

KARO_UBI_CMD() {
    karo_ubi_boot_volume
    karo_ubi_userfs_volume
    mkfs.ubifs -r ${WORKDIR}/rootfs -o ${WORKDIR}/rootfs.ubifs ${MKUBIFS_ARGS_ROOTFS}
    mkfs.ubifs -r ${WORKDIR}/bootfs -o ${WORKDIR}/boot.ubifs ${MKUBIFS_ARGS_BOOT}
    mkfs.ubifs -r ${WORKDIR}/userfs -o ${WORKDIR}/userfs.ubifs ${MKUBIFS_ARGS_USERFS}

    karo_write_ubi_config
    ubinize -o ${IMGDEPLOYDIR}/${IMAGE_NAME}.ubi ${UBINIZE_ARGS} ${WORKDIR}/ubinize-${IMAGE_NAME}.cfg

    # Deploy the individual volume images alongside the combined .ubi image
    cp ${WORKDIR}/rootfs.ubifs ${IMGDEPLOYDIR}/${BPN}-${MACHINE}.rootfs.ubifs
    cp ${WORKDIR}/boot.ubifs ${IMGDEPLOYDIR}/${BPN}-${MACHINE}.boot.ubifs
    cp ${WORKDIR}/userfs.ubifs ${IMGDEPLOYDIR}/${BPN}-${MACHINE}.userfs.ubifs
}

python do_create_flash_package:append() {
    do_copyfile("%s/%s-%s.ubi" % (d.getVar('DEPLOY_DIR_IMAGE'), d.getVar('BPN'), d.getVar('MACHINE')), d.getVar('KARO_PACKAGE_DIR'))
}
