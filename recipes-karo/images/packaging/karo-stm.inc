#
# STM32 specific flash package generation
#
# Generates the flash package directory containing the TF-A/FIP
# binaries, the uuu.auto script and the start-fastboot.sh script.
#

require karo.inc

# Set to "1" to enable STM32CubeProgrammer flashlayout generation for
# STM32MP-based machines. This includes st-machine-common-stm32mp.inc,
# which defines the boot/rootfs/userfs partition images and install
# scripts used by STM32CubeProgrammer to flash the board.
KARO_STM32_CUBE_PROGRAMMER ?= "0"

# STM32CubeProgrammer flashlayout generation
require ${@ "cubeprog/st-machine-common-stm32mp.inc" if d.getVar('KARO_STM32_CUBE_PROGRAMMER') == '1' else ""}

SRC_URI:append = " file://start-fastboot.template"

do_create_flash_package[depends] += "virtual/bootloader:do_deploy"

BOOTLOADER_BASENAME = "${SOC_FAMILY}-${MACHINE}-${BOOTSCHEME_DEFAULT}"
BOOTLOADER_BASENAME:append = "${@ bb.utils.contains('SIGN_ENABLE', '1', '_Signed', '', d)}"

KARO_UUU_FLASH_CMD_bootloader ?= "FB: flash mmc0boot0 tf-a-${BOOTLOADER_BASENAME}.stm32"
KARO_UUU_FLASH_CMD_rootfs ?= "FB: flash -raw2sparse mmc0 ${BPN}-${MACHINE}.wic.zst/*"

python do_create_flash_package() {
    from string import Template

    machine = d.getVar('MACHINE')

    img_deploy_dir = d.getVar('DEPLOY_DIR_IMAGE')
    img_dirname = d.getVar('KARO_PACKAGE_DIR')
    template_path = os.path.join(d.getVar('WORKDIR'), "start-fastboot.template")
    script_path = os.path.join(img_dirname, "start-fastboot.sh")

    bb.utils.mkdirhier(img_dirname)

    # generate uuu auto script
    karo_generate_uuu_script(d)

    tfa_name = ""
    ddrfw_name = ""
    fip_name = ""

    suffix = '_Signed' if d.getVar("SIGN_ENABLE") == "1" else ''
    type = d.getVar('TF_A_DEVICETREE')
    bootscheme = d.getVar('BOOTSCHEME_DEFAULT')
    binaries = d.getVar('BOOTLOADER_BINARIES').split()

    # copy bootloader binaries into the package directory
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

    # generate start fastboot script
    f = open(template_path, 'r')
    template_string = f.read()
    f.close()

    bb.note("Creating '%s' from '%s'" % (script_path, template_path))
    try:
        with open(script_path, 'w') as script:
            template = Template(template_string)
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
