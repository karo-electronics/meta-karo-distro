FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
inherit relative_symlinks

SRC_URI:append = "\
        file://50-firmware.rules \
"
