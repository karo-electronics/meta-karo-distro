#
# RZ/G2L specific processing
#

# create shell scripts for image flashing
addtask do_unpack before do_create_install_scripts
addtask do_create_install_scripts after do_image_complete before do_build

do_create_install_scripts[depends] += "\
    flashwriter-rzg2:do_deploy \
    virtual/bootloader:do_deploy \
"

python do_create_install_scripts() {
    from string import Template

    machine = d.getVar('MACHINE')
    rootfs_image = d.getVar('BPN')

    template_path = os.path.join(d.getVar('WORKDIR'), "install-bootloader.template")
    uuu_template_path = os.path.join(d.getVar('WORKDIR'), "uuu.auto.template")
    img_dirname = os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), d.getVar('BPN'))
    script_path = os.path.join(img_dirname, "install-bootloader.sh")
    uuu_auto_path = os.path.join(img_dirname, "uuu.auto")

    flash_cmds = "FB: flash -raw2sparse mmc0 %s-%s.wic.zst/*" % (rootfs_image, machine)

    bb.utils.mkdirhier(img_dirname)

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

    f = open(uuu_template_path, 'r')
    uuu_template_string = f.read()
    f.close()

    bb.note("Creating '%s' from '%s'" % (uuu_auto_path, uuu_template_path))
    try:
        with open(uuu_auto_path, 'w') as uuu_script:
            uuu_template = Template(uuu_template_string)

            uuu_auto = uuu_template.substitute(image=rootfs_image, machine=machine, flash_cmds=flash_cmds)
            uuu_script.write(uuu_auto)
            uuu_script.close()
    except OSError:
        bb.fatal('Unable to open %s' % (uuu_auto_path))

    do_copyfile("%s/Flash_Writer_SCIF_%s.mot" % (d.getVar('DEPLOY_DIR_IMAGE'), \
        machine), img_dirname)
    do_copyfile("%s/%s/bl2_bp-%s.bin" % (d.getVar('DEPLOY_DIR_IMAGE'), d.getVar('FIPTOOL_DIR'), \
        machine), img_dirname)
    do_copyfile("%s/fip-%s.bin" % (d.getVar('DEPLOY_DIR_IMAGE'), \
        machine), img_dirname)

    do_copyfile("%s/%s-%s.wic.zst" % \
        (d.getVar('DEPLOY_DIR_IMAGE'), rootfs_image, machine), img_dirname)
}
