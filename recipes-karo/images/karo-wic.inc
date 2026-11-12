inherit export-wic-partitions
# Partition images exported from the wic partition generation
# The seperate partition images are needed for STM32 flashlayout
# generation and also generating a RAUC update bundle.
WIC_PARTITIONS_EXPORT ?= "boot rootfs userfs"

IMAGE_INSTALL:append = " ${@bb.utils.contains('KARO_RESIZE_USERFS', '1', 'resize-userfs', '', d)}"

WKS_FILE_BASE = "karo-image"
WKS_FILE_BASE:stm32mpcommon = "karo-image-st"

WKS_FILE_BASE:append = "${@ bb.utils.contains('DISTRO_FEATURES', 'rauc', '-rauc', '', d)}"

WKS_FILE = "${WKS_FILE_BASE}.wks.in"

# Make IMAGE_FSTYPES available to wic plugins (e.g. karo-rootfs tar export)
WICVARS:append = " IMAGE_FSTYPES"

# Flash package option for including the wic image. Enabled by default.
KARO_PACKAGE_INCLUDE_WIC ??= "1"

# Copy the wic image into the flash package directory.
python do_create_flash_package:append() {
    if d.getVar('KARO_PACKAGE_INCLUDE_WIC') == "1":
        do_copyfile("%s/%s-%s.wic.zst" % (d.getVar('DEPLOY_DIR_IMAGE'),
                                          d.getVar('BPN'),
                                          d.getVar('MACHINE')),
                    d.getVar('KARO_PACKAGE_DIR'))
}
