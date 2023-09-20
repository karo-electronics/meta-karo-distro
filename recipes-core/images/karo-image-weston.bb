SUMMARY = "A very basic Wayland image with Weston desktop and terminal"

inherit features_check
require recipes-graphics/images/core-image-weston.bb
require karo-image.inc

IMAGE_FEATURES:remove = "ssh-server-dropbear"
IMAGE_FEATURES += "ssh-server-openssh"

LICENSE = "MIT"

REQUIRED_DISTRO_FEATURES = "wayland"

CORE_IMAGE_BASE_INSTALL:append = " \
        glmark2 \
        weston \
        weston-init \
        weston-examples \
"

IMAGE_INSTALL:append = " \
        gst-examples \
        libdrm \
        libdrm-tests \
        ${@bb.utils.contains('DISTRO_FEATURES', 'x11 wayland', 'weston-xwayland xterm', '', d)} \
"

IMAGE_INSTALL:append:mx6 = " \
        libdrm-etnaviv \
"

IMAGE_INSTALL:append:stm32mp1 = " \
        libdrm-etnaviv \
"

IMAGE_INSTALL:append:mx8-nxp-bsp = " \
        packagegroup-fsl-gstreamer1.0 \
"

IMAGE_INSTALL:append:mx8mm-nxp-bsp = " \
        imx-vpu-hantro-daemon \
"

IMAGE_INSTALL:append:qsxp = " \
        kernel-module-isp-vvcam \
        isp-imx \
        packagegroup-imx-isp \
"

# karo-image-weston won't fit in any of our nand modules!
IMAGE_FSTYPES:remove = "ubi"

ROOTFS_PARTITION_SIZE ?= "2097152"

QB_MEM = '${@bb.utils.contains("DISTRO_FEATURES", "opengl", "-m 512", "-m 256", d)}'
