SUMMARY = "A qt5 image with Weston desktop for use with qt creator for app development"

require karo-image-weston.bb

inherit populate_sdk_qt6_base

IMAGE_INSTALL:append = "${@ "packagegroup-qt6-imx" if "use-nxp-bsp" in d.getVar('MACHINEOVERRIDES').split(':') else "packagegroup-qt5"}"

IMAGE_FEATURES += "qtcreator-debug"
