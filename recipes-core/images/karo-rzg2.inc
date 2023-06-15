#
# RZ/G2L specific processing
#
# Define default boot partition size
BOOTFS_PARTITION_SIZE ?= "32MiB"

# create shell scripts for image flashing
addtask do_create_install_scripts after do_image_complete before do_build

python do_create_install_scripts() {
    def do_copyfile (src, destdir):
        import shutil

        bb.note("Copying %s to %s" % (src, destdir))
        shutil.copyfile(src, os.path.join(destdir, os.path.basename(src)))


    def get_part_info(index):
        bb.note("partlist='%s' index=%s" % (",".join(partlist), index))

        lastpart = index == len(partlist)

        if index == rootfs_partition_index:
            partname = "rootfs"
        else:
            i = index if index < rootfs_partition_index else index - 1
            partname = partlist[i]
            bb.note("partname[%d]='%s'" % (i, partname))
        if partname in d.getVarFlags('IMAGE_ROOTFS_SIZE'):
            size = d.getVarFlags('IMAGE_ROOTFS_SIZE')[partname]
        elif d.getVar(partname.upper() + "_PARTITION_SIZE"):
            size = d.getVar(partname.upper() + "_PARTITION_SIZE")
        elif index == rootfs_partition_index:
            size = d.getVar('IMAGE_ROOTFS_SIZE')
        elif not lastpart:
            bb.fatal("Cannot determine size for partition '%s'" % partname)

        if lastpart:
            size = "MAX"
        elif size.isnumeric():
            size = size + "KiB"

        if d.getVarFlags('PARTITION_NAMES') and partname in d.getVarFlags('PARTITION_NAMES'):
            partname = d.getVarFlag('PARTITION_NAMES', partname, True)

        bb.note("partition[%s]='%s'" % (partname, size))
        return "name=%s,size=%s;" % (partname, size)




    from string import Template

    machine = d.getVar('MACHINE')
    image = d.getVar('BPN')

    template_path = os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), "install-bootloader.template")
    uuu_template_path = os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), "uuu.auto.template")
    img_dirname = os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), d.getVar('BPN'))
    script_path = os.path.join(img_dirname, "install-bootloader.sh")
    uuu_auto_path = os.path.join(img_dirname, "uuu.auto")

    partlist = (d.getVar('PARTITIONS_IMAGE') or "").replace("karo-image-", "").split()
    mntpoints = (d.getVar('PARTITIONS_MOUNTPOINT_IMAGE') or "").split()
    if d.getVar('ROOTFS_PARTITION_INDEX') != None:
        rootfs_partition_index = d.getVar('ROOTFS_PARTITION_INDEX')
    else:
        rootfs_partition_index = 1


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
    f.close()

    bb.note("Creating '%s' from '%s'" % (uuu_auto_path, uuu_template_path))
    try:
        with open(uuu_auto_path, 'w') as uuu_script:
            uuu_template = Template(uuu_template_string)
            partitions = "start=1MiB,"
            for index in range(0, len(partlist) + 1):
                partitions += get_part_info(index)

            uuu_auto = uuu_template.substitute(image=image, machine=machine, \
                image_bootloader=d.getVar('IMAGE_BOOTLOADER'), \
                partitions=partitions)
            uuu_script.write(uuu_auto)
    except OSError:
        bb.fatal('Unable to open %s' % (uuu_auto_path))

    do_copyfile("%s/Flash_Writer_SCIF_%s.mot" % (d.getVar('DEPLOY_DIR_IMAGE'), \
        machine), img_dirname)
    do_copyfile("%s/bl2_bp-%s.srec" % (d.getVar('DEPLOY_DIR_IMAGE'), \
        machine), img_dirname)
    do_copyfile("%s/fip-%s.srec" % (d.getVar('DEPLOY_DIR_IMAGE'), \
        machine), img_dirname)
    do_copyfile("%s/bl2_bp-%s.bin" % (d.getVar('DEPLOY_DIR_IMAGE'), \
        machine), img_dirname)
    do_copyfile("%s/fip-%s.bin" % (d.getVar('DEPLOY_DIR_IMAGE'), \
        machine), img_dirname)
    do_copyfile("%s/karo-image-bootfs-%s.ext4" % \
        (d.getVar('DEPLOY_DIR_IMAGE'), machine), img_dirname)

    for partname in partlist:
        do_copyfile("%s/karo-image-%s-%s.ext4" % \
            (d.getVar('DEPLOY_DIR_IMAGE'), partname, machine), img_dirname)

    do_copyfile("%s/%s-%s.ext4" % \
        (d.getVar('DEPLOY_DIR_IMAGE'), image, machine), img_dirname)
}
