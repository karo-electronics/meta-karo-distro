# NAND flash and UBI tools
IMAGE_INSTALL:append = " mtd-utils"

# Multi-volume UBI image: the .ubi image carries three volumes:
#   boot   (vol_id 0): kernel image and DTB files
#   rootfs (vol_id 1): the root filesystem
#   userfs (vol_id 2): empty volume, autoresized into the remaining flash

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

karo_write_ubi_config() {
    cat <<EOF > ubinize-${IMAGE_NAME}.cfg
[boot]
mode=ubi
image=${WORKDIR}/boot.ubifs
vol_id=0
vol_type=dynamic
vol_name=boot
[rootfs]
mode=ubi
image=${WORKDIR}/rootfs.ubifs
vol_id=1
vol_type=dynamic
vol_name=rootfs
[userfs]
mode=ubi
image=${WORKDIR}/userfs.ubifs
vol_id=2
vol_type=dynamic
vol_name=userfs
vol_flags=autoresize
EOF
}

karo_ubi_boot_volume() {
    rm -rf "${WORKDIR}/bootfs"
    install -d "${WORKDIR}/bootfs"
    install -m 0644 "${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}" \
        "${WORKDIR}/bootfs/${KERNEL_IMAGETYPE}"
    # DTB with the overlays merged onto the carrier DTB (loaded by U-Boot)
    install -m 0644 "${DEPLOY_DIR_IMAGE}/${UBOOT_DTB_NAME}.dtb" \
        "${WORKDIR}/bootfs/"
    # Kernel DTBs and overlays
    for dtb in ${IMAGE_BOOT_FILES}; do
        install -m 0644 "${DEPLOY_DIR_IMAGE}/$dtb" "${WORKDIR}/bootfs/"
    done
}

karo_ubi_userfs_volume() {
    rm -rf "${WORKDIR}/userfs"
    install -d "${WORKDIR}/userfs"
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
    ubinize -o ${IMGDEPLOYDIR}/${IMAGE_NAME}.ubi ${UBINIZE_ARGS} ubinize-${IMAGE_NAME}.cfg
    mv ubinize-${IMAGE_NAME}.cfg ${IMGDEPLOYDIR}/
}

python do_create_flash_package:append() {
    # NAND: kernel, DTB and UBI goes beside uuu.auto
    do_copyfile("%s/%s" % (d.getVar('DEPLOY_DIR_IMAGE'), d.getVar('KERNEL_IMAGETYPE')), d.getVar('KARO_PACKAGE_DIR'))
    do_copyfile("%s/%s.dtb" % (d.getVar('DEPLOY_DIR_IMAGE'), d.getVar('UBOOT_DTB_NAME')), d.getVar('KARO_PACKAGE_DIR'))
    do_copyfile("%s/%s-%s.ubi" % (d.getVar('DEPLOY_DIR_IMAGE'), d.getVar('BPN'), d.getVar('MACHINE')), d.getVar('KARO_PACKAGE_DIR'))
}
