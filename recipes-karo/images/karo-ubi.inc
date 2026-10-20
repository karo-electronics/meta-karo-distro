# kernel-base recommends the versioned kernel-image-<ver> package
BAD_RECOMMENDATIONS:append = " kernel-image-*"
# NAND flash and UBI tools
IMAGE_INSTALL:append = " mtd-utils"

# UBI sizing
IMAGE_ROOTFS_ALIGNMENT = "128"
IMAGE_ROOTFS_EXTRA_SPACE = "${@2*128}"
IMAGE_OVERHEAD_FACTOR = "1"
IMAGE_ROOTFS_SIZE ?= "${@${ROOTFS_PARTITION_SIZE}-(${IMAGE_ROOTFS_EXTRA_SPACE})}"

MKUBIFS_LEB_SIZE = "126976"
MKUBIFS_LEB_CNT ?= "${@int(((${IMAGE_ROOTFS_SIZE} + ${IMAGE_ROOTFS_EXTRA_SPACE}) * 1024 + ${MKUBIFS_LEB_SIZE} - 1) / ${MKUBIFS_LEB_SIZE})}"
MKUBIFS_ARGS ?= "-F -x zlib -m 2048 -e ${MKUBIFS_LEB_SIZE} -c ${MKUBIFS_LEB_CNT}"
UBINIZE_ARGS ?= "-m 2048 -s 2048 -p 128KiB"
UBI_VOLNAME = "rootfs"

# Multi-volume UBI image: the .ubi image carries three volumes:
#   boot   (vol_id 0): kernel image and DTB files
#   rootfs (vol_id 1): the root filesystem built by the ubifs image type
#   userfs (vol_id 2): empty volume, autoresized into the remaining flash
KARO_UBI_BOOT_VOLNAME ?= "boot"
KARO_UBI_USERFS_VOLNAME ?= "userfs"

# The boot volume is built from the kernel and bootloader deploy
# artifacts, not from the rootfs (BAD_RECOMMENDATIONS above keeps the
# kernel out of the rootfs).
do_image_ubi[depends] += "virtual/kernel:do_deploy virtual/bootloader:do_deploy"

karo_ubi_stage_boot() {
	rm -rf "${WORKDIR}/boot-volume"
	install -d "${WORKDIR}/boot-volume"
	install -m 0644 "${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}" \
		"${WORKDIR}/boot-volume/${KERNEL_IMAGETYPE}"
	# DTB with the overlays merged onto the carrier DTB (loaded by U-Boot)
	install -m 0644 "${DEPLOY_DIR_IMAGE}/${UBOOT_DTB_NAME}.dtb" \
		"${WORKDIR}/boot-volume/"
	# Kernel DTBs and overlays
	for dtbf in ${KERNEL_DEVICETREE}; do
		install -m 0644 "${DEPLOY_DIR_IMAGE}/$(basename $dtbf)" "${WORKDIR}/boot-volume/"
	done
}

karo_write_ubi_config() {
	cat <<EOF > ubinize-${IMAGE_NAME}.cfg
[${KARO_UBI_BOOT_VOLNAME}]
mode=ubi
image=${WORKDIR}/boot.ubifs
vol_id=0
vol_type=${UBI_VOLTYPE}
vol_name=${KARO_UBI_BOOT_VOLNAME}
[${UBI_VOLNAME}]
mode=ubi
image=${IMGDEPLOYDIR}/${IMAGE_NAME}.ubifs
vol_id=1
vol_type=${UBI_VOLTYPE}
vol_name=${UBI_VOLNAME}
[${KARO_UBI_USERFS_VOLNAME}]
mode=ubi
image=${WORKDIR}/userfs.ubifs
vol_id=2
vol_type=${UBI_VOLTYPE}
vol_name=${KARO_UBI_USERFS_VOLNAME}
vol_flags=autoresize
EOF
}

# image_types.bbclass is inherited through inherit_defer and is therefore
# parsed after this file, so a plain IMAGE_CMD:ubi assignment here would be
# overridden by its single-volume definition. Apply the override from a
# RecipePostKeyExpansion handler instead: it runs after the deferred classes
# are parsed but before image.bbclass generates the do_image_ubi task from
# IMAGE_CMD:ubi.
addhandler karo_ubi_set_image_cmd
karo_ubi_set_image_cmd[eventmask] = "bb.event.RecipePostKeyExpansion"
python karo_ubi_set_image_cmd() {
    if "ubi" in (d.getVar('IMAGE_FSTYPES') or '').split():
        d.setVar('IMAGE_CMD:ubi', d.getVar('KARO_UBI_IMAGE_CMD', False))
}

KARO_UBI_IMAGE_CMD () {
	karo_ubi_stage_boot
	rm -rf "${WORKDIR}/userfs-volume"
	install -d "${WORKDIR}/userfs-volume"
	mkfs.ubifs -r ${WORKDIR}/boot-volume -o ${WORKDIR}/boot.ubifs ${MKUBIFS_ARGS_BOOT}
	mkfs.ubifs -r ${WORKDIR}/userfs-volume -o ${WORKDIR}/userfs.ubifs ${MKUBIFS_ARGS_USERFS}
	karo_write_ubi_config
	ubinize -o ${IMGDEPLOYDIR}/${IMAGE_NAME}.ubi ${UBINIZE_ARGS} ubinize-${IMAGE_NAME}.cfg
	mv ubinize-${IMAGE_NAME}.cfg ${IMGDEPLOYDIR}/
}

MKUBIFS_BOOT_LEB_CNT ?= "${@int((${BOOTFS_PARTITION_SIZE} * 1024 + ${MKUBIFS_LEB_SIZE} - 1) / ${MKUBIFS_LEB_SIZE})}"
MKUBIFS_ARGS_BOOT ?= "-x zlib -m 2048 -e ${MKUBIFS_LEB_SIZE} -c ${MKUBIFS_BOOT_LEB_CNT}"
MKUBIFS_USERFS_LEB_CNT ?= "${@int((${USERFS_PARTITION_SIZE} * 1024 + ${MKUBIFS_LEB_SIZE} - 1) / ${MKUBIFS_LEB_SIZE})}"
MKUBIFS_ARGS_USERFS ?= "-x zlib -m 2048 -e ${MKUBIFS_LEB_SIZE} -c ${MKUBIFS_USERFS_LEB_CNT}"

python do_create_flash_package:append() {
    # NAND: kernel, DTB and UBI goes beside uuu.auto
    do_copyfile("%s/%s" % (d.getVar('DEPLOY_DIR_IMAGE'), d.getVar('KERNEL_IMAGETYPE')), d.getVar('KARO_PACKAGE_DIR'))
    do_copyfile("%s/%s.dtb" % (d.getVar('DEPLOY_DIR_IMAGE'), d.getVar('UBOOT_DTB_NAME')), d.getVar('KARO_PACKAGE_DIR'))
    do_copyfile("%s/%s-%s.ubi" % (d.getVar('DEPLOY_DIR_IMAGE'), d.getVar('BPN'), d.getVar('MACHINE')), d.getVar('KARO_PACKAGE_DIR'))
}
