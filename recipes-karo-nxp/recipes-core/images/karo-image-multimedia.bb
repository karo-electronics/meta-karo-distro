SUMMARY = "An image with full multimedia support derived from imx-image-multimedia"

require recipes-core/images/karo-image.inc
#require recipes-fsl/images/imx-image-multimedia.bb

bluetooth_pkg = "${@bb.utils.contains('DISTRO_FEATURES','bluethooth','','packagegroup-tools-bluetooth',d)}"

CORE_IMAGE_EXTRA_INSTALL_remove = "${@bb.utils.contains('DISTRO_FEATURES','bluethooth','','packagegroup-tools-bluetooth',d)}"

# canutils conflicts with can-utils from packagegroup-fsl-tools-testapps
IMAGE_INSTALL_remove = "canutils"
