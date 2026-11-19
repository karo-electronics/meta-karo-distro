#
# i.MX specific flash package generation (NXP BSP machines)
#
# Generates the flash package directory containing the U-Boot binaries
# and the uuu.auto script.
#
# Included after packaging/karo.inc, which carries the common flash
# package logic; do not require it again here.
#

do_create_flash_package[depends] += "virtual/bootloader:do_deploy"
do_create_flash_package[depends] += "imx-boot:do_deploy"

KARO_UUU_FLASH_CMD_bootloader ?= "FB: flash mmc0boot0 u-boot-${MACHINE}-default.bin"
KARO_UUU_FLASH_CMD_rootfs ?= "FB: flash -raw2sparse -bmap ${IMAGE_LINK_NAME}.wic.bmap mmc0 ${IMAGE_LINK_NAME}.wic.zst/*"

python do_create_flash_package() {
    import os
    import shutil

    machine = d.getVar('MACHINE')
    overrides = d.getVar('MACHINEOVERRIDES').split(':')
    image_bootloader = d.getVar('IMAGE_BOOTLOADER')

    img_deploy_dir = d.getVar('DEPLOY_DIR_IMAGE')
    img_dirname = d.getVar('KARO_PACKAGE_DIR')

    bb.utils.mkdirhier(img_dirname)

    # generate uuu auto script
    karo_generate_uuu_script(d)

    # copy bootloader binaries into the package directory
    for uboot_type in d.getVar('UBOOT_CONFIG').split():
        src = os.path.join(img_deploy_dir, "%s-%s-%s.bin" % (image_bootloader, machine, uboot_type))
        dst = os.path.join(img_dirname, "u-boot-%s-%s.bin" % (machine, uboot_type))
        if not os.path.exists(src):
            # The imx-boot recipe deploys one image per build target.
            # Copy it under the targetless name so that we have
            # unified binary names accross all machines.
            for target in (d.getVar('IMXBOOT_TARGETS') or '').split():
                candidate = "%s-%s" % (src, target)
                if os.path.exists(candidate):
                    src = candidate
                    break
        bb.note("Copying %s to %s" % (src, dst))
        shutil.copyfile(src, dst)
}
