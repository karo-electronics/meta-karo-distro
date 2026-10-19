#
# i.MX specific flash package generation (NXP BSP machines)
#
# Generates the flash package directory containing the U-Boot binaries
# and the uuu.auto script.
#

require karo.inc

do_create_flash_package[depends] += "virtual/bootloader:do_deploy"

UUU_TEMPLATE_NAME:mx6-nxp-bsp ?= "${@bb.utils.contains('MACHINE_FEATURES', 'nand', 'uuu-nand.auto.template', 'uuu.auto.template', d)}"

KARO_UUU_FLASH_CMD_bootloader ?= "FB: flash mmc0boot0 ${IMAGE_BOOTLOADER}-${MACHINE}-default.bin"
KARO_UUU_FLASH_CMD_bootloader:mx6-nxp-bsp ?= "${@karo_mx6_uuu_bootloader_cmd(d)}"
KARO_UUU_FLASH_CMD_rootfs ?= "FB: flash -raw2sparse mmc0 ${BPN}-${MACHINE}.wic.zst/*"
KARO_UUU_FLASH_CMD_rootfs:mx6-nxp-bsp ?= "${@karo_mx6_uuu_rootfs_cmd(d)}"

python do_create_flash_package() {
    machine = d.getVar('MACHINE')
    overrides = d.getVar('MACHINEOVERRIDES').split(':')
    image_bootloader = d.getVar('IMAGE_BOOTLOADER')

    dtb_name = "%s.dtb" % d.getVar('DTB_BASENAME')
    # For NAND modules the DTB overlays are merged onto the carrier DTB
    if 'nand' in d.getVar('MACHINE_FEATURES').split():
        dtb_name = "%s.dtb" % d.getVar('UBOOT_DTB_NAME')

    img_deploy_dir = d.getVar('DEPLOY_DIR_IMAGE')
    img_dirname = d.getVar('KARO_PACKAGE_DIR')

    bb.utils.mkdirhier(img_dirname)

    # generate uuu auto script
    karo_generate_uuu_script(d, extra_subs={
        'image_bootloader': image_bootloader,
        'dtb_name': dtb_name,
    })

    # copy bootloader binaries into the package directory
    for type in d.getVar('UBOOT_CONFIG').split():
        do_copyfile("%s/%s-%s-%s.bin" % (img_deploy_dir, image_bootloader, machine, type), img_dirname)

    # i.MX6 RM Table 8-25: eMMC IVT at 1 KiB; mmc0boot0 is flashed at 0
    if 'mx6-nxp-bsp' in overrides and 'nand' not in d.getVar('MACHINE_FEATURES').split():
        with open("%s/%s-%s-default.bin" % (img_deploy_dir, image_bootloader, machine), 'rb') as f:
            bl_data = f.read()
        with open("%s/%s-%s-default-padded.bin" % (img_dirname, image_bootloader, machine), 'wb') as f:
            f.write(b'\x00' * 1024 + bl_data)
}

def karo_mx6_uuu_bootloader_cmd(d):
    image = "%s-%s-default" % (d.getVar('IMAGE_BOOTLOADER'), d.getVar('MACHINE'))
    if 'nand' not in d.getVar('MACHINE_FEATURES').split():
        # eMMC: the IVT must sit at the 1 KiB offset (RM Table 8-25)
        return "FB: flash mmc0boot0 %s-padded.bin" % image
    # NAND: written last, with the FCB and DBBT (nandbcb init), so an
    # interrupted run stays USB-recoverable. ${loadaddr} and ${filesize}
    # are U-Boot runtime variables, set by the preceding 'FB: download'.
    return "FB: download -f %s.bin\nFB: ucmd nandbcb init ${loadaddr} nandboot ${filesize}" % image

def karo_mx6_uuu_rootfs_cmd(d):
    image = "%s-%s" % (d.getVar('BPN'), d.getVar('MACHINE'))
    if 'nand' not in d.getVar('MACHINE_FEATURES').split():
        return "FB: flash -raw2sparse mmc0 %s.wic.zst/*" % image
    flash_cmd = "FB: erase linux\nFB: flash linux %s\n" % d.getVar('KERNEL_IMAGETYPE')
    flash_cmd += "FB: erase dtb\nFB: flash dtb %s.dtb\n" % d.getVar('UBOOT_DTB_NAME')
    flash_cmd += "FB: erase rootfs\nFB: flash rootfs %s.ubi" % image
    return flash_cmd
