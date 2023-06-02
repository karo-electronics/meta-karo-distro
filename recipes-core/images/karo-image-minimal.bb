SUMMARY = "A minimal Linux system without graphics support."

require karo-image.inc 
require karo-minimal.inc

IMAGE_ROOTFS_MAXSIZE ?= "${@bb.utils.contains('MACHINE_FEATURES',"nand","65536","",d)}"

python extend_recipe_sysroot:append() {
    if d.getVar('DISTRO') != 'karo-minimal':
        raise_sanity_error("cannot build karo-image-minimal with '%s' DISTRO" % d.getVar('DISTRO'), d)
}
