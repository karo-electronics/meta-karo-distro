SUMMARY = "A qt6 image with Weston desktop for use with qt creator for app development"

require karo-image-weston.bb

inherit populate_sdk_qt6

IMAGE_INSTALL:append:use-nxp-bsp = " packagegroup-qt6-imx"
