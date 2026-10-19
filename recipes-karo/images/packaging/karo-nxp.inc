#
# i.MX specific flash package generation (NXP BSP machines)
#
# Generates the flash package directory containing the U-Boot binaries
# and the uuu.auto script.
#

require karo.inc

do_create_flash_package[depends] += "virtual/bootloader:do_deploy"
# NAND copies the deployed kernel and DTB
do_create_uuu_auto_script[depends] += "${@ 'virtual/kernel:do_deploy' if 'nand' in d.getVar('MACHINE_FEATURES').split() else ''}"

KARO_UUU_FLASH_CMD_bootloader ?= "FB: flash mmc0boot0 ${IMAGE_BOOTLOADER}-${MACHINE}-default.bin"
KARO_UUU_FLASH_CMD_bootloader:mx6-nxp-bsp ?= "${@ '' if 'nand' in d.getVar('MACHINE_FEATURES').split() else 'FB: flash mmc0boot0 ${IMAGE_BOOTLOADER}-${MACHINE}-default-padded.bin'}"
KARO_UUU_FLASH_CMD_rootfs ?= "FB: flash -raw2sparse mmc0 ${BPN}-${MACHINE}.wic.zst/*"
KARO_UUU_FLASH_CMD_rootfs:mx6-nxp-bsp ?= "${@ '' if 'nand' in d.getVar('MACHINE_FEATURES').split() else 'FB: flash -raw2sparse mmc0 ${BPN}-${MACHINE}.wic.zst/*'}"

python do_create_flash_package() {
    machine = d.getVar('MACHINE')
    overrides = d.getVar('MACHINEOVERRIDES').split(':')
    image_bootloader = d.getVar('IMAGE_BOOTLOADER')

    dtb_name = "%s.dtb" % d.getVar('DTB_BASENAME')
    # For nand modules the DTB overlays are merged onto the carrier DTB
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
