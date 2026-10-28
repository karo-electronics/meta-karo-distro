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

ROOTFS_PARTITION_SIZE ?= "2097152"

QB_MEM = '${@bb.utils.contains("DISTRO_FEATURES", "opengl", "-m 512", "-m 256", d)}'
