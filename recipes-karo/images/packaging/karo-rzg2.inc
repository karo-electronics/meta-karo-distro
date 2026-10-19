#
# RZ/G2L specific flash package generation
#
# Generates the flash package directory containing the Flash Writer,
# the ARM Trusted Firmware binaries, the uuu.auto script and the
# install-bootloader.sh script.
#

require karo.inc

SRC_URI:append = " file://install-bootloader.template"

do_create_flash_package[depends] += "\
    flashwriter-rzg2:do_deploy \
    virtual/bootloader:do_deploy \
"

# The bootloader is re-programmed by the commands in the uuu.auto
# template itself, so the flash command for the bootloader step
# is empty by default.
KARO_UUU_FLASH_CMD_bootloader ?= ""
KARO_UUU_FLASH_CMD_rootfs ?= "FB: flash -raw2sparse mmc0 ${BPN}-${MACHINE}.wic.zst/*"

python do_create_flash_package() {
    from string import Template

    machine = d.getVar('MACHINE')
    fiptool_dir = d.getVar('FIPTOOL_DIR')
    img_deploy_dir = d.getVar('DEPLOY_DIR_IMAGE')
    img_dirname = d.getVar('KARO_PACKAGE_DIR')
    template_path = os.path.join(d.getVar('WORKDIR'), "install-bootloader.template")
    script_path = os.path.join(img_dirname, "install-bootloader.sh")

    bb.utils.mkdirhier(img_dirname)

    karo_generate_uuu_script(d)

    # copy bootloader binaries into the package directory
    do_copyfile("%s/Flash_Writer_SCIF_%s.mot" % (img_deploy_dir, machine), img_dirname)
    do_copyfile("%s/%s/bl2_bp-%s.bin" % (img_deploy_dir, fiptool_dir, machine), img_dirname)
    do_copyfile("%s/fip-%s.bin" % (img_deploy_dir, machine), img_dirname)

    # generate bootloader install script
    f = open(template_path, 'r')
    template_string = f.read()
    f.close()

    bb.note("Creating '%s' from '%s'" % (script_path, template_path))
    try:
        with open(script_path, 'w') as script:
            template = Template(template_string)
            s = template.substitute(machine=machine)
            script.write(s)
            script.close()
    except OSError:
        bb.fatal('Unable to open %s' % (script_path))

    os.chmod(script_path, 0o755)
}
