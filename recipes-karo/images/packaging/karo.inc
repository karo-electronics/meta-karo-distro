#
# Common logic for generating the flash package directory
# (bootloader binaries plus flash scripts).
#

FILESEXTRAPATHS:prepend := "${THISDIR}/uuu-script-templates:"

UUU_TEMPLATE_NAME ??= "uuu.auto.template"

def karo_layer_file(d, filename):
    import bb.fetch2

    path = bb.fetch2.localpath("file://%s" % filename, d)
    if not os.path.exists(path):
        bb.fatal("Cannot find '%s' in FILESPATH" % filename)
    return path

# Rerun do_create_flash_package when the template changes.
do_create_flash_package[file-checksums] = "${@karo_layer_file(d, d.getVar('UUU_TEMPLATE_NAME'))}:True"

# Directory where the flash package is assembled
KARO_PACKAGE_DIR = "${DEPLOY_DIR_IMAGE}/${BPN}"

# Ordered list of flash steps rendered into the '### Flashing partitions'
# section of uuu.auto. The uuu command of each step is configured with
# KARO_UUU_FLASH_CMD_<step>; a step whose command is unset is skipped.
# Steps are rendered in list order, one command per
# line (a command may itself span multiple lines).
#
# Example:
#   KARO_UUU_FLASH_STEPS = "rootfs bootloader"
#   KARO_UUU_FLASH_CMD_bootloader = "FB: flash mmc0boot0 u-boot.bin"
#   KARO_UUU_FLASH_CMD_rootfs = "FB: flash -raw2sparse mmc0 image.wic.zst/*"
#
# Append a step to extend the script, remove one from the list
# to disable it, or override a command to change
# the flash behavior for a specific machine or image.
KARO_UUU_FLASH_STEPS ??= "rootfs bootloader"

do_create_flash_package[dirs] = "${DEPLOY_DIR_IMAGE}"

python () {
    # Schedule the flash script generation: image recipes (which have a
    # do_image_complete task) run it after the image is complete,
    # everything else runs it after do_unpack.
    after = "do_unpack"
    if d.getVarFlag('do_image_complete', 'task'):
        after = "do_image_complete do_unpack"
    bb.build.addtask('do_create_flash_package', 'do_build', after, d)
}

def do_copyfile (src, destdir):
    import shutil

    bb.note("Copying %s to %s" % (src, destdir))
    try:
        shutil.copy(src, os.path.join(destdir, os.path.basename(src)))
    except OSError as e:
        bb.fatal("Failed to copy %s to %s: %s" % (src, destdir, e))

def karo_uuu_flash_cmds(d):
    """
    Return the flash commands for the '### Flashing partitions' section
    of uuu.auto: one command per configured step from
    KARO_UUU_FLASH_STEPS, in list order.
    """
    cmds = []
    for step in (d.getVar('KARO_UUU_FLASH_STEPS') or '').split():
        cmd = (d.getVar('KARO_UUU_FLASH_CMD_%s' % step) or '').strip()
        if not cmd:
            continue
        cmds.append(cmd)
    return '\n'.join(cmds)

def karo_generate_uuu_script(d, extra_subs=None):
    """
    Render the uuu.auto.template from the layer (see karo_layer_file)
    into the flash package directory as uuu.auto. Available
    placeholders: $image, $machine, $flash_cmds and everything passed
    in extra_subs.
    """
    from string import Template

    if not d.getVar('KARO_UUU_FLASH_STEPS'):
        return

    img_dirname = d.getVar('KARO_PACKAGE_DIR')
    template_path = karo_layer_file(d, d.getVar('UUU_TEMPLATE_NAME'))
    uuu_auto_path = os.path.join(img_dirname, "uuu.auto")

    subs = {
        'image': d.getVar('BPN'),
        'machine': d.getVar('MACHINE'),
        'flash_cmds': karo_uuu_flash_cmds(d),
    }
    if extra_subs:
        subs.update(extra_subs)

    bb.utils.mkdirhier(img_dirname)

    # read uuu template
    f = open(template_path, 'r')
    uuu_template_string = f.read()
    f.close()

    # generate uuu auto script
    bb.note("Creating '%s' from '%s'" % (uuu_auto_path, template_path))
    try:
        with open(uuu_auto_path, 'w') as uuu_script:
            uuu_script.write(Template(uuu_template_string).substitute(subs))
    except OSError:
        bb.fatal('Unable to open %s' % (uuu_auto_path))

# Export the SBOM into the package directory. The postfuncs take
# effect wherever a do_create_image_sbom_spdx task exists.
python do_export_sbom() {
    machine = d.getVar('MACHINE')
    package_name = d.getVar('BPN')
    img_dirname = d.getVar('KARO_PACKAGE_DIR')

    bb.utils.mkdirhier(img_dirname)
    do_copyfile("%s/%s-%s.spdx.json" % \
        (d.getVar('SPDXIMAGEDEPLOYDIR'), package_name, machine), img_dirname)
}
do_create_image_sbom_spdx[postfuncs] += "do_export_sbom"
do_create_image_sbom_spdx_setscene[postfuncs] += "do_export_sbom"
