#
# STM32 specific processing
#

# create shell scripts for image flashing
addtask do_unpack before do_create_install_scripts
addtask do_create_install_scripts after do_image_complete before do_build

do_create_install_scripts[depends] += "virtual/bootloader:do_deploy"

python do_create_install_scripts() {
    from string import Template
    from itertools import product

    binaries_basename = d.getVar('BINARIES_TARGET_BASENAME')

    machine = d.getVar('MACHINE')
    rootfs_image = d.getVar('BPN')

    img_dirname = os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), d.getVar('BPN'))
    uuu_template_path = os.path.join(d.getVar('WORKDIR'), "uuu.auto.template")
    uuu_auto_path = os.path.join(img_dirname, "uuu.auto")
    fastboot_template_path = os.path.join(d.getVar('WORKDIR'), "start-fastboot.template")
    script_path = os.path.join(img_dirname, "start-fastboot.sh")

    flash_cmds = "FB: flash mmc0boot0 tf-a-%s.stm32\n" % binaries_basename
    flash_cmds += "FB: flash -raw2sparse mmc0 %s-%s.wic.zst/*" % (rootfs_image, machine)

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
            uuu_auto = uuu_template.substitute(image=rootfs_image, machine=machine, flash_cmds=flash_cmds)
            uuu_script.write(uuu_auto)
            uuu_script.close()
    except OSError:
        bb.fatal('Unable to open %s' % (uuu_auto_path))

    # copy trusted firmware and bootloader to uuu install dir
    tfa_name = ""
    ddrfw_name = ""
    fip_name = ""

    types = d.getVar('FLASHLAYOUT_TYPE_LABELS').split()
    configs = d.getVar('FLASHLAYOUT_CONFIG_LABELS').split()
    bootschemes = d.getVar('FLASHLAYOUT_BOOTSCHEME_LABELS').split()
    for type, config, bootscheme in product(types, configs, bootschemes):
        partitions = expand_var('FLASHLAYOUT_PARTITION_LABELS', bootscheme, config, '', d).split()
        for partition in partitions:
            if partition == "none":
                continue

            fn = get_binaryname(type, bootscheme, config, partition, d)

            if partition == "fsbl-boot":
                tfa_name = fn
            elif partition == "fip-ddr":
                ddrfw_name = fn
            elif partition == "fip-boot":
                fip_name = fn

            if fn != "none":
                do_copyfile("%s" % os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), fn), img_dirname)

    do_copyfile("%s" % os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), "fastboot.img"), img_dirname)
    do_copyfile("%s/%s-%s.wic.zst" % (d.getVar('DEPLOY_DIR_IMAGE'), rootfs_image, machine), img_dirname)

    bb.note("Creating '%s' from '%s'" % (script_path, fastboot_template_path))
    try:
        with open(script_path, 'w') as script:
            template = Template(fastboot_template_string)
            s = template.substitute(
                machine=machine, 
                tfa_name=tfa_name,
                fip_name=fip_name,
                ddrfw_name=ddrfw_name
            )
            script.write(s)
            script.close()
    except OSError:
        bb.fatal('Unable to open %s' % (script_path))

    os.chmod(script_path, 0o755)
}
