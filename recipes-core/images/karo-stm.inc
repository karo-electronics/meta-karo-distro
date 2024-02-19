#
# STM32 specific processing
#

# create shell scripts for image flashing
addtask do_create_install_scripts after do_image_complete before do_build

do_create_install_scripts[depends] += "\
    virtual/trusted-firmware-a:do_deploy \
    virtual/bootloader:do_deploy \
"

python do_create_install_scripts() {
    def do_copyfile (src, destdir):
        import shutil

        bb.note("Copying %s to %s" % (src, destdir))
        shutil.copyfile(src, os.path.join(destdir, os.path.basename(src)))

    bb.note("DEPLOY_DIR_IMAGE='%s'" % d.expand("${DEPLOY_DIR_IMAGE}"))
    bb.note("BPN='%s'" % d.expand("${BPN}"))

    from string import Template

    soc_family = d.getVar('SOC_FAMILY')
    bootscheme = d.getVar('FLASHLAYOUT_BOOTSCHEME_LABELS')

    machine = d.getVar('MACHINE')
    rootfs_image = d.getVar('BPN')
    rfs_size = d.getVar('ROOTFS_PARTITION_SIZE')

    template_path = os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), "install-bootloader.template")
    uuu_template_path = os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), "uuu.auto.template")
    img_dirname = os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), d.getVar('BPN'))
    uuu_auto_path = os.path.join(img_dirname, "uuu.auto")

    partlist = (d.getVar('PARTITIONS_IMAGE') or "").replace("karo-image-", "").split()
    mntpoints = (d.getVar('PARTITIONS_MOUNTPOINT_IMAGE') or "").split()

    if d.getVar('ROOTFS_PARTITION_INDEX') != None:
        rootfs_partition_index = d.getVar('ROOTFS_PARTITION_INDEX')
    else:
        rootfs_partition_index = 1

    partitions = "start=1MiB,name=fip,size=2MiB;"
    flash_cmds = "FB: flash fip fip-%s-%s-%s.bin\n" % (soc_family, machine, bootscheme)
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
            uuu_auto = uuu_template.substitute(image=rootfs_image, machine=machine, \
                soc_family=soc_family, bootscheme=bootscheme, partitions=partitions, \
                flash_cmds=flash_cmds)
            uuu_script.write(uuu_auto)
            uuu_script.close()
    except OSError:
        bb.fatal('Unable to open %s' % (uuu_auto_path))

    # copy trusted firmware and bootloader to uuu install dir
    for type in d.getVar('FLASHLAYOUT_BOOTSCHEME_LABELS').split():
        do_copyfile("%s/tf-a-%s-%s-%s.stm32" % (d.getVar('DEPLOY_DIR_IMAGE'), \
            soc_family, machine, type), img_dirname)
    for type in d.getVar('FLASHLAYOUT_BOOTSCHEME_LABELS').split():
        do_copyfile("%s/fip-%s-%s-%s.bin" % (d.getVar('DEPLOY_DIR_IMAGE'), \
            soc_family, machine, type), img_dirname)

    # copy rootfs image file
    do_copyfile("%s/%s-%s.ext4" % (d.getVar('DEPLOY_DIR_IMAGE'), rootfs_image, machine), img_dirname)

    # copy other partition images
    for partname in partlist:
        do_copyfile("%s/karo-image-%s-%s.ext4" % (d.getVar('DEPLOY_DIR_IMAGE'), \
            partname, machine), img_dirname)
}
