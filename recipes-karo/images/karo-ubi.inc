# NAND flash and UBI tools
IMAGE_INSTALL:append = " mtd-utils"

# Multi-volume UBI image: the .ubi image carries three volumes:
#   boot   (vol_id 0): kernel image and DTB files
#   rootfs (vol_id 1): the root filesystem
#   userfs (vol_id 2): volume mounted at /usr/local, autoresized into the remaining flash
BOOTFS_PARTITION_SIZE ?= "16384"
ROOTFS_PARTITION_SIZE ?= "65536"
USERFS_PARTITION_SIZE ?= "16384"

# UBI geometry: the UBI device is attached to the rootfs partition of
# CONFIG_MTDPARTS_DEFAULT; UBI_AREA_SIZE must match its size (KiB).
UBI_AREA_SIZE ?= "126080"
MKUBIFS_PEB_SIZE = "131072"
MKUBIFS_LEB_SIZE = "126976"

# Maximum number of LEBs available to UBI volumes: the PEBs of the
# UBI area minus the PEBs UBI reserves for the volume table, wear
# leveling, EBA and fastmap.
MKUBIFS_RESERVED_LEB_CNT ?= "24"
MKUBIFS_MAX_LEB_CNT = "${@int(${UBI_AREA_SIZE} * 1024 / ${MKUBIFS_PEB_SIZE}) - ${MKUBIFS_RESERVED_LEB_CNT}}"

MKUBIFS_ARGS_BOOTFS ?= "-F -x zlib -m 2048 -e ${MKUBIFS_LEB_SIZE} -c ${MKUBIFS_MAX_LEB_CNT}"
MKUBIFS_ARGS_ROOTFS ?= "-F -x zlib -m 2048 -e ${MKUBIFS_LEB_SIZE} -c ${MKUBIFS_MAX_LEB_CNT}"
MKUBIFS_ARGS_USERFS ?= "-F -x zlib -m 2048 -e ${MKUBIFS_LEB_SIZE} -c ${MKUBIFS_MAX_LEB_CNT}"

UBINIZE_ARGS ?= "-m 2048 -s 2048 -p 128KiB"

# The boot volume is built from the kernel and bootloader deploy
# artifacts, not from the rootfs (BAD_RECOMMENDATIONS above keeps the
# kernel out of the rootfs).
do_image_ubi[depends] += "virtual/kernel:do_deploy virtual/bootloader:do_deploy"

# ubinize volume layout template
KARO_UBINIZE_CFG ?= "ubinize-karo.cfg.in"
KARO_UBINIZE_CFG_PATH = "${@karo_layer_file(d, d.getVar('KARO_UBINIZE_CFG'))}"
do_image_ubi[file-checksums] = "${KARO_UBINIZE_CFG_PATH}:True"

karo_ubi_prep_bootfs() {
    rm -rf "${WORKDIR}/bootfs"
    install -d "${WORKDIR}/bootfs"
    # Kernel image and DTBs
    for file in ${IMAGE_BOOT_FILES}; do
        install -m 0644 "${DEPLOY_DIR_IMAGE}/$file" "${WORKDIR}/bootfs/"
    done
}

karo_ubi_prep_userfs() {
    rm -rf "${WORKDIR}/userfs"
    install -d "${WORKDIR}/userfs"
    # Move the rootfs /usr/local content into the userfs volume; the
    # rootfs itself keeps an empty /usr/local as the mount point
    if [ -d "${IMAGE_ROOTFS}/usr/local" ]; then
        find "${IMAGE_ROOTFS}/usr/local" -mindepth 1 -maxdepth 1 \
            -exec mv -t "${WORKDIR}/userfs" {} +
    fi
}

karo_ubi_tar_archives() {
    for fstype in ${IMAGE_FSTYPES}; do
        case "$fstype" in
            tar.*) ;;
            *) continue ;;
        esac
        for vol in boot rootfs userfs; do
            vol_dir="${vol%fs}fs"
            tar -caf "${WORKDIR}/$vol.$fstype" -C "${WORKDIR}/$vol_dir" .
            cp "${WORKDIR}/$vol.$fstype" "${IMGDEPLOYDIR}/${BPN}-${MACHINE}.$vol.$fstype"
        done
    done
}

karo_ubi_volumes() {
    mkfs.ubifs -r ${WORKDIR}/rootfs -o ${WORKDIR}/rootfs.ubifs ${MKUBIFS_ARGS_ROOTFS}
    cp ${WORKDIR}/rootfs.ubifs ${IMGDEPLOYDIR}/${BPN}-${MACHINE}.rootfs.ubifs
    mkfs.ubifs -r ${WORKDIR}/bootfs -o ${WORKDIR}/boot.ubifs ${MKUBIFS_ARGS_BOOTFS}
    cp ${WORKDIR}/boot.ubifs ${IMGDEPLOYDIR}/${BPN}-${MACHINE}.boot.ubifs
    mkfs.ubifs -r ${WORKDIR}/userfs -o ${WORKDIR}/userfs.ubifs ${MKUBIFS_ARGS_USERFS}
    cp ${WORKDIR}/userfs.ubifs ${IMGDEPLOYDIR}/${BPN}-${MACHINE}.userfs.ubifs
}

karo_write_ubi_config() {
    sed -e "s|@WORKDIR@|${WORKDIR}|g" \
        -e "s|@BOOTFS_PARTITION_SIZE@|${BOOTFS_PARTITION_SIZE}|g" \
        -e "s|@ROOTFS_PARTITION_SIZE@|${ROOTFS_PARTITION_SIZE}|g" \
        -e "s|@USERFS_PARTITION_SIZE@|${USERFS_PARTITION_SIZE}|g" \
        "${KARO_UBINIZE_CFG_PATH}" > ${WORKDIR}/ubinize-${IMAGE_NAME}.cfg
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
    # Prepare volume contents
    karo_ubi_prep_bootfs
    karo_ubi_prep_userfs
    # Generate and deploy tar archives
    karo_ubi_tar_archives
    # Generate and deploy the individual volume images
    karo_ubi_volumes
    # Generate the ubi image containing all volumes
    karo_write_ubi_config
    ubinize -o ${IMGDEPLOYDIR}/${IMAGE_NAME}.ubi ${UBINIZE_ARGS} ${WORKDIR}/ubinize-${IMAGE_NAME}.cfg
}

python do_create_flash_package:append() {
    do_copyfile("%s/%s-%s.ubi" % (d.getVar('DEPLOY_DIR_IMAGE'), d.getVar('BPN'), d.getVar('MACHINE')), d.getVar('KARO_PACKAGE_DIR'))
}
