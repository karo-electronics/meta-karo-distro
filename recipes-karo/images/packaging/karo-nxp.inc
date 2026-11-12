#
# i.MX specific flash package generation (NXP BSP machines)
#
# Generates the flash package directory containing the U-Boot binaries
# and the uuu.auto script.
#

do_create_flash_package[depends] += "virtual/bootloader:do_deploy"

KARO_UUU_FLASH_CMD_bootloader ?= "FB: flash mmc0boot0 ${IMAGE_BOOTLOADER}-${MACHINE}-default.bin"
KARO_UUU_FLASH_CMD_bootloader:mx6-nxp-bsp ?= "${@karo_mx6_uuu_bootloader_cmd(d)}"
KARO_UUU_FLASH_CMD_rootfs ?= "FB: flash -raw2sparse mmc0 ${BPN}-${MACHINE}.wic.zst/*"
KARO_UUU_FLASH_CMD_rootfs:mx6-nxp-bsp ?= "${@karo_mx6_uuu_rootfs_cmd(d)}"

python do_create_flash_package() {
    machine = d.getVar('MACHINE')
    image_bootloader = d.getVar('IMAGE_BOOTLOADER')

    img_deploy_dir = d.getVar('DEPLOY_DIR_IMAGE')
    img_dirname = d.getVar('KARO_PACKAGE_DIR')

    bb.utils.mkdirhier(img_dirname)

    # generate uuu auto script
    karo_generate_uuu_script(d, extra_subs={
        'image_bootloader': image_bootloader,
    })

    # copy bootloader binaries into the package directory
    for type in d.getVar('UBOOT_CONFIG').split():
        do_copyfile("%s/%s-%s-%s.bin" % (img_deploy_dir, image_bootloader, machine, type), img_dirname)
}

def karo_mx6_uuu_bootloader_cmd(d):
    image = "%s-%s-default" % (d.getVar('IMAGE_BOOTLOADER'), d.getVar('MACHINE'))
    if 'nand' not in d.getVar('MACHINE_FEATURES').split():
        return "FB: flash mmc0boot0 %s.bin" % image
    # NAND: written last, with the FCB and DBBT (nandbcb init), so an
    # interrupted run stays USB-recoverable. ${loadaddr} and ${filesize}
    # are U-Boot runtime variables, set by the preceding 'FB: download'.
    return "FB: download -f %s.bin\nFB: ucmd nandbcb init ${loadaddr} nandboot ${filesize}" % image

def karo_mx6_uuu_rootfs_cmd(d):
    image = "%s-%s" % (d.getVar('BPN'), d.getVar('MACHINE'))
    if 'nand' not in d.getVar('MACHINE_FEATURES').split():
        return "FB: flash -raw2sparse mmc0 %s.wic.zst/*" % image
    return "FB: erase rootfs\nFB: flash rootfs %s.ubi" % image
