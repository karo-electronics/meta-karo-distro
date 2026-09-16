#
# STM32 specific processing
#

BOOTLOADER_BASENAME = "${SOC_FAMILY}-${MACHINE}-${BOOTSCHEME_DEFAULT}"
BOOTLOADER_BASENAME:append = "${@ bb.utils.contains('SIGN_ENABLE', '1', '_Signed', '', d)}"

# create shell scripts for image flashing
addtask do_unpack before do_create_install_scripts
addtask do_create_install_scripts after do_image_complete before do_build

do_create_install_scripts[depends] += "virtual/bootloader:do_deploy"

python do_create_install_scripts() {
    from string import Template

    machine = d.getVar('MACHINE')
    rootfs_image = d.getVar('BPN')

    img_deploy_dir = d.getVar('DEPLOY_DIR_IMAGE')
    img_dirname = os.path.join(img_deploy_dir, d.getVar('BPN'))
    uuu_template_path = os.path.join(d.getVar('WORKDIR'), "uuu.auto.template")
    uuu_auto_path = os.path.join(img_dirname, "uuu.auto")
    fastboot_template_path = os.path.join(d.getVar('WORKDIR'), "start-fastboot.template")
    script_path = os.path.join(img_dirname, "start-fastboot.sh")

    bootloader_basename = d.getVar('BOOTLOADER_BASENAME')

    flash_cmds = "FB: flash mmc0boot0 tf-a-%s.stm32" % (bootloader_basename)
    flash_cmds += "\nFB: flash -raw2sparse mmc0 %s-%s.wic.zst/*" % (rootfs_image, machine)

    bb.utils.mkdirhier(img_dirname) 

    do_copyfile("%s/%s-%s.wic.zst" % \
        (img_deploy_dir, rootfs_image, machine), img_dirname)   

    # generate uuu auto script 
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

    tfa_name = ""
    ddrfw_name = ""
    fip_name = ""

    suffix = '_Signed' if d.getVar("SIGN_ENABLE") == "1" else ''
    type = d.getVar('TF_A_DEVICETREE')
    bootscheme = d.getVar('BOOTSCHEME_DEFAULT')
    binaries = d.getVar('BOOTLOADER_BINARIES').split()

    # copy bootloader binaries to uuu install dir
    for binary in binaries:
        label, ext = os.path.splitext(binary)

        fn = f"{label}-{type}-{bootscheme}{suffix}{ext}"
        do_copyfile(os.path.join(img_deploy_dir, fn), img_dirname)

        initial_bootscheme = d.getVarFlag('INITIAL_BOOTSCHEME', label)
        if initial_bootscheme:
            fn = f"{label}-{type}-{initial_bootscheme}{suffix}{ext}"
            do_copyfile(os.path.join(img_deploy_dir, fn), img_dirname)

        if label == "tf-a":
            tfa_name = fn
        elif label == "fip-ddr":
            ddrfw_name = fn
        elif label == "fip":
            fip_name = fn

    do_copyfile("%s" % os.path.join(img_deploy_dir, "fastboot.img"), img_dirname)

    # generate bootloader install script
    f = open(fastboot_template_path, 'r')
    fastboot_template_string = f.read()
    f.close()

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
