SUMMARY = "A very basic X11 image with a terminal"

require recipes-graphics/images/core-image-x11.bb
require karo-image.inc

LICENSE = "MIT"

CORE_IMAGE_BASE_INSTALL:append = " \
    gtk+3-demo \
    matchbox-terminal \
    glmark2 \
"

IMAGE_FEATURES:append = " \
    ssh-server-openssh \
    hwcodecs \
"

IMAGE_INSTALL:append = " \
    packagegroup-core-x11-xserver \
    gst-examples \
    libdrm \
    libdrm-tests \
    xterm \
"

IMAGE_INSTALL:append:mx6 = " \
    libdrm-etnaviv \
"

IMAGE_INSTALL:append:mx8-nxp-bsp = " \
    packagegroup-fsl-gstreamer1.0 \
"

IMAGE_INSTALL:append:mx8mm-nxp-bsp = " \
    imx-vpu-hantro-daemon \
"

IMAGE_INSTALL:append:qsxp = " \
    ${@bb.utils.contains('DISTRO_FEATURES', 'csi-camera', 'kernel-module-isp-vvcam isp-imx packagegroup-imx-isp', '', d)} \
"

# karo-image-x11 won't fit in any of our nand modules!
IMAGE_FSTYPES:remove = "ubi"

python extend_recipe_sysroot:append() {
    if d.getVar('DISTRO') != 'karo-x11':
        raise_sanity_error("cannot build 'karo-image-x11' with DISTRO '%s'" % d.getVar('DISTRO'), d)
}

ROOTFS_PARTITION_SIZE ?= "2097152"
