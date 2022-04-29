#
# RZ/G2L specific processing
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

    machine = d.getVar('MACHINE')
    image = d.getVar('BPN')

    template_path = os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), "install-bootloader.template")
    uuu_template_path = os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), "uuu.auto.template")
    img_dirname = os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), d.getVar('BPN'))
    script_path = os.path.join(img_dirname, "install-bootloader.sh")
    uuu_auto_path = os.path.join(img_dirname, "uuu.auto")

    bb.utils.mkdirhier(img_dirname)

    f = open(template_path, 'r')
    template_string = f.read()

    bb.note("Creating '%s' from '%s'" % (script_path, template_path))
    try:
        with open(script_path, 'w') as script:
            template = Template(template_string)
            s = template.substitute(image=image, machine=machine)
            script.write(s)
    except OSError:
        bb.fatal('Unable to open %s' % (script_path))

    os.chmod(script_path, 0o755)

    f = open(uuu_template_path, 'r')
    uuu_template_string = f.read()

    bb.note("Creating '%s' from '%s'" % (uuu_auto_path, uuu_template_path))
    try:
        with open(uuu_auto_path, 'w') as uuu_script:
            uuu_template = Template(uuu_template_string)
            uuu_auto = uuu_template.substitute(image=image, machine=machine)
            uuu_script.write(uuu_auto)
    except OSError:
        bb.fatal('Unable to open %s' % (uuu_auto_path))

    do_copyfile("%s/Flash_Writer_SCIF_TXRZ_DDR3L_1GB.mot" % (d.getVar('DEPLOY_DIR_IMAGE')), \
        img_dirname)
    do_copyfile("%s/bl2_bp-%s.srec" % (d.getVar('DEPLOY_DIR_IMAGE'), \
        machine), img_dirname)
    do_copyfile("%s/fip-%s.srec" % (d.getVar('DEPLOY_DIR_IMAGE'), \
        machine), img_dirname)
    do_copyfile("%s/karo-image-bootfs-%s.ext4" % \
        (d.getVar('DEPLOY_DIR_IMAGE'), machine), img_dirname)
    do_copyfile("%s/%s-%s.ext4" % \
        (d.getVar('DEPLOY_DIR_IMAGE'), image, machine), img_dirname)
}
