IMAGE_BASENAME = "karo-image-x11"
SUMMARY = "A very basic X11 image with a terminal"

require recipes-graphics/images/core-image-x11.bb
require karo-image.inc

IMAGE_FEATURES:append = " \
        hwcodecs \
        package-management \
        splash \
        ssh-server-dropbear \
        x11-base \
"

IMAGE_INSTALL:append = " \
        packagegroup-core-x11-xserver \
"

LICENSE = "MIT"

DEPENDS:append:use-nxp-bsp = " xf86-video-imx-vivante"

# karo-image-x11 won't fit in any of our nand modules!
IMAGE_FSTYPES:remove = "ubi"

python extend_recipe_sysroot:append() {
    if d.getVar('DISTRO') != 'karo-x11':
        raise_sanity_error("cannot build 'karo-image-x11' with DISTRO '%s'" % d.getVar('DISTRO'), d)
}

ROOTFS_PARTITION_SIZE = "2097152"
