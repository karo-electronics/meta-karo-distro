#
# i.MX specific processing
#

# create uuu.auto file for image
addtask do_create_uuu_auto_script after do_image_complete before do_build

do_create_uuu_auto_script[depends] += "virtual/bootloader:do_deploy"

python do_create_uuu_auto_script() {
    bb.note("DEPLOY_DIR_IMAGE='%s'" % d.expand("${DEPLOY_DIR_IMAGE}"))
    bb.note("BPN='%s'" % d.expand("${BPN}"))

    from string import Template

    machine = d.getVar('MACHINE')
    rootfs_image = d.getVar('BPN')

    uuu_template_path = os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), "uuu.auto.template")
    img_dirname = os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), d.getVar('BPN'))
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

    f = open(uuu_template_path, 'r')
    uuu_template_string = f.read()
    f.close()

    bb.note("Creating '%s' from '%s'" % (uuu_auto_path, uuu_template_path))
    try:
        with open(uuu_auto_path, 'w') as uuu_script:
            uuu_template = Template(uuu_template_string)
            uuu_auto = uuu_template.substitute(image=rootfs_image, machine=machine,
                partitions=partitions, flash_cmds=flash_cmds,
                image_bootloader=d.getVar('IMAGE_BOOTLOADER'))
            uuu_script.write(uuu_auto)
            uuu_script.close()
    except OSError:
        bb.fatal('Unable to open %s' % (uuu_auto_path))

    for type in d.getVar('UBOOT_CONFIG').split():
        do_copyfile("%s/%s-%s-%s.%s" % (d.getVar('DEPLOY_DIR_IMAGE'), \
            d.getVar('IMAGE_BOOTLOADER'), machine, type, d.getVar('UBOOT_SUFFIX')), \
            img_dirname)

    do_copyfile("%s/%s-%s.ext4" % \
        (d.getVar('DEPLOY_DIR_IMAGE'), rootfs_image, machine), img_dirname)

    for partname in partlist:
        do_copyfile("%s/karo-image-%s-%s.ext4" % (d.getVar('DEPLOY_DIR_IMAGE'), \
            partname, machine), img_dirname)
}
