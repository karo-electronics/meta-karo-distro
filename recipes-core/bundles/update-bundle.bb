DESCRIPTION = "RAUC demo bundle generator"

inherit bundle

RAUC_BUNDLE_COMPATIBLE ?= "${MACHINE}"
RAUC_BUNDLE_VERSION = "v00"
RAUC_BUNDLE_DESCRIPTION = "Ka-Ro RAUC Demo Bundle"

RAUC_BUNDLE_FORMAT = "verity"

RAUC_BUNDLE_SLOTS = "boot rootfs" 

RAUC_SLOT_rootfs = "karo-image-minimal"
RAUC_SLOT_rootfs[fstype] = "ext4"

RAUC_SLOT_boot = "karo-image-bootfs"
RAUC_SLOT_boot[fstype] = "ext4"

RAUC_KEY_FILE = "${THISDIR}/files/development-1.key.pem"
RAUC_CERT_FILE = "${THISDIR}/files/development-1.cert.pem"
