inherit export-wic-partitions
WIC_PARTITIONS_EXPORT = "boot rootfs userfs"

IMAGE_FSTYPES = "wic.zst"

IMAGE_INSTALL:append = " kernel-devicetree kernel-image"

WKS_FILE_BASE = "karo-image"
WKS_FILE_BASE:stm32mpcommon = "karo-image-st"

WKS_FILE_BASE:append = "${@ bb.utils.contains('DISTRO_FEATURES', 'rauc', '-rauc', '', d)}"

WKS_FILE = "${WKS_FILE_BASE}.wks.in"
