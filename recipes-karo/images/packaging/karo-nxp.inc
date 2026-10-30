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

# Name of the bootloader flash image deployed by the imx-boot recipe.
# meta-freescale used to provide this variable.
IMAGE_BOOTLOADER ?= "imx-boot"

KARO_UUU_FLASH_CMD_bootloader ?= "FB: flash mmc0boot0 ${IMAGE_BOOTLOADER}-${MACHINE}-default.bin"
KARO_UUU_FLASH_CMD_rootfs ?= "FB: flash -raw2sparse mmc0 ${BPN}-${MACHINE}.wic.zst/*"

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
    karo_generate_uuu_script(d, extra_subs={
        'image_bootloader': image_bootloader,
    })

    # copy bootloader binaries into the package directory
    for uboot_type in d.getVar('UBOOT_CONFIG').split():
        src = "%s/%s-%s-%s.bin" % (img_deploy_dir, image_bootloader, machine, uboot_type)
        dst = os.path.join(img_dirname, "%s-%s-%s.bin" % (image_bootloader, machine, uboot_type))
        if not os.path.exists(src):
            # Since wrynose, the imx-boot recipe deploys one image per
            # build target, e.g. imx-boot-<machine>-<type>.bin-flash_a55.
            # Copy it under the targetless name so that the uuu script
            # and the flash package layout stay the same for all
            # machines.
            for target in (d.getVar('IMXBOOT_TARGETS') or '').split():
                candidate = "%s-%s" % (src, target)
                if os.path.exists(candidate):
                    src = candidate
                    break
        bb.note("Copying %s to %s" % (src, dst))
        shutil.copyfile(src, dst)
}
