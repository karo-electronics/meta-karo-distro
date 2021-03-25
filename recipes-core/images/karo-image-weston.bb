SUMMARY = "A very basic Wayland image with Weston desktop and terminal"

inherit features_check
require karo-image.inc

IMAGE_FEATURES += " \
	          hwcodecs \
	          package-management \
	          splash \
		  ssh-server-openssh \
"

LICENSE = "MIT"

REQUIRED_DISTRO_FEATURES = "wayland"

IMAGE_INSTALL_append = " \
		       clutter-1.0-examples \
		       glmark2 \
		       gst-examples \
		       gtk+3-demo \
		       libdrm \
		       libdrm-tests \
		       libdrm-kms \
		       libdrm-etnaviv \
		       weston \
		       weston-init \
		       weston-examples \
		       ${@bb.utils.contains('DISTRO_FEATURES', 'x11 wayland', 'weston-xwayland xterm', '', d)} \
"

# karo-image-weston won't fit in any of our nand modules!
IMAGE_FSTYPES_remove = "ubi"

ROOTFS_PARTITION_SIZE = "2097152"

QB_MEM = '${@bb.utils.contains("DISTRO_FEATURES", "opengl", "-m 512", "-m 256", d)}'
