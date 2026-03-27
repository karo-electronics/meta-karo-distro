FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

FILES:${PN}:append = " ${sysconfdir}/edac/edac-driver"

SRC_URI:append = " file://edac-driver"

RDEPENDS:${PN}:append = " \
    perl-module-file-spec \
"

do_install:append() {
    install -D ${WORKDIR}/edac-driver ${D}${sysconfdir}/edac/edac-driver
}
