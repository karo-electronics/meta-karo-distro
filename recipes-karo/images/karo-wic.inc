inherit export-wic-partitions
# Partition images exported from the wic partition generation
# The seperate partition images are needed for STM32 flashlayout generation and
# also generating a RAUC update bundle. They are included in the flash package
# by default (see KARO_PACKAGE_INCLUDE_PARTITIONS in packaging/karo.inc).
WIC_PARTITIONS_EXPORT ?= "boot rootfs userfs"
IMAGE_INSTALL:append = " ${@bb.utils.contains('KARO_RESIZE_USERFS', '1', 'resize-userfs', '', d)}"


WKS_FILE_BASE = "karo-image"
WKS_FILE_BASE:stm32mpcommon = "karo-image-st"

WKS_FILE_BASE:append = "${@ bb.utils.contains('DISTRO_FEATURES', 'rauc', '-rauc', '', d)}"

WKS_FILE = "${WKS_FILE_BASE}.wks.in"

# Make IMAGE_FSTYPES available to wic plugins (e.g. karo-rootfs tar export)
WICVARS:append = " IMAGE_FSTYPES"

# Flash package content options. The wic image is included per default,
# while the exported wic partition images are not included.
KARO_PACKAGE_INCLUDE_WIC ??= "1"
KARO_PACKAGE_INCLUDE_PARTITIONS ??= "0"

# Copy the wic image and the exported wic partition
# images into the flash package directory.
python do_create_flash_package:append() {
    if d.getVar('KARO_PACKAGE_INCLUDE_WIC') == "1":
        do_copyfile("%s/%s-%s.wic.zst" % (d.getVar('DEPLOY_DIR_IMAGE'),
                                          d.getVar('BPN'),
                                          d.getVar('MACHINE')),
                    d.getVar('KARO_PACKAGE_DIR'))

    if d.getVar('KARO_PACKAGE_INCLUDE_PARTITIONS') == "1":
        for partition in d.getVar('WIC_PARTITIONS_EXPORT').split():
            do_copyfile("%s/%s-%s.%s.*" % (d.getVar('DEPLOY_DIR_IMAGE'),
                                           d.getVar('BPN'),
                                           d.getVar('MACHINE'),
                                           partition),
                        d.getVar('KARO_PACKAGE_DIR'))
}
