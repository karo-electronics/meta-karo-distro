PACKAGE_INSTALL += "${@ bb.utils.contains('DISTRO_FEATURES', 'rauc', "u-boot-script", "", d)}"
