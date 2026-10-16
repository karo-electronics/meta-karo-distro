#
# i.MX specific flash package generation (NXP BSP machines)
#
# Generates the flash package directory containing the U-Boot binaries
# and the uuu.auto script.
#

require karo.inc

do_create_flash_package[depends] += "virtual/bootloader:do_deploy"

KARO_UUU_FLASH_CMD_bootloader ?= "FB: flash mmc0boot0 ${IMAGE_BOOTLOADER}-${MACHINE}-default.bin"
KARO_UUU_FLASH_CMD_rootfs ?= "FB: flash -raw2sparse mmc0 ${BPN}-${MACHINE}.wic.zst/*"

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
