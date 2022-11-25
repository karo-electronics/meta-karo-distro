#
# STM32 specific processing
#

# create shell scripts for image flashing
addtask do_create_install_scripts after do_image_complete before do_build

python do_create_install_scripts() {
    def do_copyfile (src, destdir):
        import shutil

        bb.note("Copying %s to %s" % (src, destdir))
        shutil.copyfile(src, os.path.join(destdir, os.path.basename(src)))

    bb.note("DEPLOY_DIR_IMAGE='%s'" % d.expand("${DEPLOY_DIR_IMAGE}"))
    bb.note("BPN='%s'" % d.expand("${BPN}"))

    from string import Template

    soc_family = d.getVar('SOC_FAMILY')
    machine = d.getVar('MACHINE')
    image = d.getVar('BPN')

    template_path = os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), "install-bootloader.template")
    uuu_template_path = os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), "uuu.auto.template")
    img_dirname = os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), d.getVar('BPN'))
    uuu_auto_path = os.path.join(img_dirname, "uuu.auto")

    bb.utils.mkdirhier(img_dirname)

    f = open(uuu_template_path, 'r')
    uuu_template_string = f.read()

    bb.note("Creating '%s' from '%s'" % (uuu_auto_path, uuu_template_path))
    try:
        with open(uuu_auto_path, 'w') as uuu_script:
            uuu_template = Template(uuu_template_string)
            uuu_auto = uuu_template.substitute(image=image, machine=machine, soc_family=soc_family)
            uuu_script.write(uuu_auto)
    except OSError:
        bb.fatal('Unable to open %s' % (uuu_auto_path))

    do_copyfile(os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), "tf-a-%s-%s-serialboot.stm32" %
                             (soc_family, machine)), img_dirname)
    do_copyfile(os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), "tf-a-%s-%s-trusted.stm32" %
                             (soc_family, machine)), img_dirname)
    do_copyfile(os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), "u-boot-mfg.bin"),
                img_dirname)
    do_copyfile(os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), "u-boot-trusted.bin"),
                img_dirname)
    do_copyfile(os.path.join(d.getVar('DEPLOY_DIR_IMAGE'),
                             "karo-image-bootfs-%s.ext4" % machine), img_dirname)
    do_copyfile(os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), "%s-%s.ext4" %
                             (image, machine)), img_dirname)
}