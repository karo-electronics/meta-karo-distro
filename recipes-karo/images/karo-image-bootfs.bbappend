PACKAGE_INSTALL += "${@ "u-boot-script" if 'rauc' in d.getVar('DISTRO_FEATURES').split() else ""}"
