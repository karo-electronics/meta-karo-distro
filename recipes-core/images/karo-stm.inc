#
# STM32 specific processing
#

# create shell scripts for image flashing
addtask do_unpack after do_image_complete before do_create_install_scripts
addtask do_create_install_scripts after do_image_complete before do_build

do_create_install_scripts[depends] += "virtual/bootloader:do_deploy"

python do_create_install_scripts() {
    from string import Template

    soc_family = d.getVar('SOC_FAMILY')
    bootscheme = d.getVar('FLASHLAYOUT_BOOTSCHEME_LABELS')

    machine = d.getVar('MACHINE')
    rootfs_image = d.getVar('BPN')
    rfs_size = d.getVar('ROOTFS_PARTITION_SIZE')

    img_dirname = os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), d.getVar('BPN'))

    uuu_template_path = os.path.join(d.getVar('WORKDIR'), "uuu.auto.template")
    uuu_auto_path = os.path.join(img_dirname, "uuu.auto")

    fastboot_template_path = os.path.join(d.getVar('WORKDIR'), "start-fastboot.template")
    script_path = os.path.join(img_dirname, "start-fastboot.sh")

    if 'rauc' in d.getVar('DISTRO_FEATURES').split():
        if d.getVar('DISTRO') == 'karo-minimal':
            rauc = True
        else:
            rauc = False
    else:
        rauc = False

    partlist = (d.getVar('PARTITIONS_IMAGE') or "").replace("karo-image-", "").split()
    mntpoints = (d.getVar('PARTITIONS_MOUNTPOINT_IMAGE') or "").split()

    if d.getVar('ROOTFS_PARTITION_INDEX') != None:
        rootfs_partition_index = d.getVar('ROOTFS_PARTITION_INDEX')
    else:
        rootfs_partition_index = 1

    partitions = "start=1MiB,name=fip,type=19d5df83-11b0-457b-be2c-7559c13142a5,size=2MiB;"
    flash_cmds = "FB: flash mmc0boot0 tf-a-%s-%s-%s.bin\n" % (soc_family, machine, bootscheme)
    flash_cmds += "FB: flash fip fip-%s-%s-%s.bin\n" % (soc_family, machine, bootscheme)
    dlm = ""
    for index in range(0, len(partlist) + 1):
        (partname, img, size) = get_part_info(d, partlist, index)
        partitions += "name=%s,size=%s;" % (partname, size)
        flash_cmds += dlm + "FB: flash -raw2sparse %s %s-%s.ext4" % (partname, img, machine)
        dlm = "\n"

    if rauc:
        partitions += "name=fipB,type=19D5DF83-11B0-457b-BE2C-7559C13142A5,size=2MiB;"
        flash_cmds += dlm + "FB: flash fipB fip-%s-%s-%s.bin\n" % (soc_family, machine, bootscheme)
        dlm = ""
        for index in range(0, len(partlist) + 1):
            (partname, img, size) = get_part_info(d, partlist, index)
            partitions += "name=%sB,size=%s;" % (partname, size)
            flash_cmds += dlm + "FB: flash -raw2sparse %sB %s-%s.ext4" % (partname, img, machine)
            dlm = "\n"

    bb.utils.mkdirhier(img_dirname)

    f = open(fastboot_template_path, 'r')
    fastboot_template_string = f.read()
    f.close()

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
    for type in d.getVar('FLASHLAYOUT_TYPE_LABELS').split():
        for config in d.getVar('FLASHLAYOUT_CONFIG_LABELS').split():
            for bootscheme in d.getVar('FLASHLAYOUT_BOOTSCHEME_LABELS').split():
                partitions = expand_var('FLASHLAYOUT_PARTITION_LABELS', bootscheme, config, '', d)
                for partition in partitions.split():
                    if partition == "none":
                        continue
                    if partition == "none":
                        continue
                    if partition == "fsbl-boot":
                        tfa_name = get_binaryname(type, bootscheme, config, partition, d);
                    if partition == "fip-boot":
                        fip_name = get_binaryname(type, bootscheme, config, partition, d);

                    fn = get_binaryname(type, bootscheme, config, partition, d);
                    if fn != "none":
                        do_copyfile("%s" % os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), fn), img_dirname)
    do_copyfile("%s" % os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), "fastboot.img"), img_dirname)

    bb.note("Creating '%s' from '%s'" % (script_path, fastboot_template_path))
    try:
        with open(script_path, 'w') as script:
            template = Template(fastboot_template_string)
            s = template.substitute(machine=machine, soc_family=soc_family, \
                bootscheme=bootscheme, tfa_name=tfa_name, fip_name=fip_name)
            script.write(s)
            script.close()
    except OSError:
        bb.fatal('Unable to open %s' % (script_path))

    os.chmod(script_path, 0o755)
}
