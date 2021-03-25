IMAGE_BASENAME = "karo-image-x11"
SUMMARY = "A very basic X11 image with a terminal"

inherit features_check
require karo-image.inc

IMAGE_FEATURES_append = " \
                         hwcodecs \
                         package-management \
                         splash \
                         ssh-server-dropbear \
                         x11-base \
"

IMAGE_INSTALL_append = " \
                        packagegroup-core-x11-xserver \
"
LICENSE = "MIT"

REQUIRED_DISTRO_FEATURES = "x11"

# karo-image-x11 won't fit in any of our nand modules!
IMAGE_FSTYPES_remove = "ubi"

QB_MEM = '${@bb.utils.contains("DISTRO_FEATURES", "opengl", "-m 512", "-m 256", d)}'

python extend_recipe_sysroot_append() {
    if d.getVar('DISTRO') != 'karo-x11':
        raise_sanity_error("cannot build 'karo-image-x11' with DISTRO '%s'" % d.getVar('DISTRO'), d)
}

ROOTFS_PARTITION_SIZE = "2097152"
