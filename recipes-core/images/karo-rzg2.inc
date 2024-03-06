#
# RZ/G2L specific processing
#
# Define default boot partition size
BOOTFS_PARTITION_SIZE ?= "64MiB"

do_create_install_scripts[depends] += "\
    uuu-script-template:do_deploy \
    flashwriter-rzg2:do_deploy \
    virtual/trusted-firmware-a:do_deploy \
    virtual/bootloader:do_deploy \
"

# create shell scripts for image flashing
addtask do_create_install_scripts after do_image_complete before do_build

python do_create_install_scripts() {
    from string import Template

    machine = d.getVar('MACHINE')
    rootfs_image = d.getVar('BPN')

    template_path = os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), "install-bootloader.template")
    uuu_template_path = os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), "uuu.auto.template")
    img_dirname = os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), d.getVar('BPN'))
    script_path = os.path.join(img_dirname, "install-bootloader.sh")
    uuu_auto_path = os.path.join(img_dirname, "uuu.auto")

    partlist = (d.getVar('PARTITIONS_IMAGE') or "").replace("karo-image-", "").split()
    mntpoints = (d.getVar('PARTITIONS_MOUNTPOINT_IMAGE') or "").split()

    partitions = "start=1MiB,"
    flash_cmds = ""
    dlm = ""
    for index in range(0, len(partlist) + 1):
        (partname, img, size) = get_part_info(d, partlist, index)
        partitions += "name=%s,size=%s;" % (partname, size)
        flash_cmds += dlm + "FB: flash -raw2sparse %s %s-%s.ext4" % (partname, img, machine)
        dlm = "\n"

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

            uuu_auto = uuu_template.substitute(image=rootfs_image, machine=machine, \
                image_bootloader=d.getVar('IMAGE_BOOTLOADER'), \
                partitions=partitions, flash_cmds=flash_cmds)
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

    for partname in partlist:
        do_copyfile("%s/karo-image-%s-%s.ext4" % \
            (d.getVar('DEPLOY_DIR_IMAGE'), partname, machine), img_dirname)

    do_copyfile("%s/%s-%s.ext4" % \
        (d.getVar('DEPLOY_DIR_IMAGE'), rootfs_image, machine), img_dirname)
}
