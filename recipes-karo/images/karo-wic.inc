inherit export-wic-partitions
WIC_PARTITIONS_EXPORT ?= "boot rootfs userfs"

WKS_FILE_BASE = "karo-image"
WKS_FILE_BASE:stm32mpcommon = "karo-image-st"

WKS_FILE_BASE:append = "${@ bb.utils.contains('DISTRO_FEATURES', 'rauc', '-rauc', '', d)}"

WKS_FILE = "${WKS_FILE_BASE}.wks.in"

# Make IMAGE_FSTYPES available to wic plugins (e.g. karo-rootfs tar export)
WICVARS:append = " IMAGE_FSTYPES"
