SUMMARY = "A qt6 image with Weston desktop for use with qt creator for app development"

require karo-image-weston.bb

INHERIT += "${@ "populate_sdk_qt6_base" if d.getVar('DISTRO') != "karo-minimal" else ""}"

IMAGE_INSTALL:append = "${@ "packagegroup-qt6-imx" if "use-nxp-bsp" in d.getVar('MACHINEOVERRIDES').split(':') else "packagegroup-qt6"}"

IMAGE_FEATURES += "qtcreator-debug"

export CMAKE_BUILD_PARALLEL_LEVEL = "${@ d.getVar('BB_NUMBER_THREADS') if d.getVar('BB_NUMBER_THREADS') != None else ""}"
