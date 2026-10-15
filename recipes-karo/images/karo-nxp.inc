#
# i.MX specific processing
#

# create uuu.auto file for image
addtask do_create_uuu_auto_script after do_image_complete before do_build
addtask do_unpack before do_create_uuu_auto_script

do_create_uuu_auto_script[depends] += "virtual/bootloader:do_deploy"
# NAND copies the deployed kernel and DTB
do_create_uuu_auto_script[depends] += "${@ 'virtual/kernel:do_deploy' if 'nand' in d.getVar('MACHINE_FEATURES').split() else ''}"

python do_create_uuu_auto_script() {
    bb.note("DEPLOY_DIR_IMAGE='%s'" % d.expand("${DEPLOY_DIR_IMAGE}"))
    bb.note("BPN='%s'" % d.expand("${BPN}"))

    from string import Template

    machine = d.getVar('MACHINE')
    rootfs_image = d.getVar('BPN')
    overrides = d.getVar('MACHINEOVERRIDES').split(':')
    nand = 'nand' in d.getVar('MACHINE_FEATURES').split()
    uuu_template_name = d.getVar('UUU_TEMPLATE_NAME')
    uboot_suffix = d.getVar('UBOOT_SUFFIX')

    uuu_template_path = os.path.join(d.getVar('WORKDIR'), uuu_template_name)
    img_dirname = os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), d.getVar('BPN'))
    uuu_auto_path = os.path.join(img_dirname, "uuu.auto")

    image_bootloader=d.getVar('IMAGE_BOOTLOADER')
    dtb_name = "%s.dtb" % d.getVar('DTB_BASENAME')

    # For nand modules the DTB overlays are merged onto the carrier DTB
    if nand and d.getVar('KARO_BASEBOARD'):
        dtb_name = "%s-%s.dtb" % (d.getVar('DTB_BASENAME'), d.getVar('KARO_BASEBOARD'))

    bb.utils.mkdirhier(img_dirname)

    flash_cmds = ""
    if 'mx6-nxp-bsp' not in overrides:
        flash_cmds = "FB: flash mmc0boot0 %s-%s-default.%s\n" % (image_bootloader, machine, uboot_suffix)

    if nand:
        # NAND: kernel, DTB and UBI goes beside uuu.auto
        for name in (d.getVar('KERNEL_IMAGETYPE'), dtb_name,
                     "%s-%s.ubi" % (rootfs_image, machine)):
            do_copyfile("%s/%s" % (d.getVar('DEPLOY_DIR_IMAGE'), name), img_dirname)
    else:
        # MMC: wic goes beside uuu.auto
        flash_cmds += "FB: flash -raw2sparse mmc0 %s-%s.wic.zst/*" % (rootfs_image, machine)
        do_copyfile("%s/%s-%s.wic.zst" % \
            (d.getVar('DEPLOY_DIR_IMAGE'), rootfs_image, machine), img_dirname)

    # generate uuu auto script 
    f = open(uuu_template_path, 'r')
    uuu_template_string = f.read()
    f.close()

    bb.note("Creating '%s' from '%s'" % (uuu_auto_path, uuu_template_path))
    try:
        with open(uuu_auto_path, 'w') as uuu_script:
            uuu_template = Template(uuu_template_string)
            uuu_auto = uuu_template.substitute(
                image=rootfs_image,
                machine=machine,
                flash_cmds=flash_cmds,
                dtb_name=dtb_name,
                image_bootloader=image_bootloader
            )
            uuu_script.write(uuu_auto)
            uuu_script.close()
    except OSError:
        bb.fatal('Unable to open %s' % (uuu_auto_path))

    # copy bootloader binaries to uuu install dir
    for type in d.getVar('UBOOT_CONFIG').split():
        do_copyfile("%s/%s-%s-%s.%s" % (d.getVar('DEPLOY_DIR_IMAGE'), \
            d.getVar('IMAGE_BOOTLOADER'), machine, type, uboot_suffix), \
            img_dirname)
}
