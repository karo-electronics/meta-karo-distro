UUU_TEMPLATE_NAME ?= "uuu-nand.auto.template"

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

python do_create_flash_package:append() {
    # NAND: kernel, DTB and UBI goes beside uuu.auto
    do_copyfile("%s/%s" % (d.getVar('DEPLOY_DIR_IMAGE'), d.getVar('KERNEL_IMAGETYPE')), d.getVar('KARO_PACKAGE_DIR'))
    do_copyfile("%s/%s.dtb" % (d.getVar('DEPLOY_DIR_IMAGE'), d.getVar('UBOOT_DTB_NAME')), d.getVar('KARO_PACKAGE_DIR'))
    do_copyfile("%s/%s-%s.ubi" % (d.getVar('DEPLOY_DIR_IMAGE'), d.getVar('BPN'), d.getVar('MACHINE')), d.getVar('KARO_PACKAGE_DIR'))
}
