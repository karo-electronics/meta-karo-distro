# The Edge TPU runtime for Google's Coral chips.
#
# Upstream release 'edgetpu_runtime_20210119'.
DESCRIPTION = "The Edge TPU runtime for Google's Coral chip"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${PN}/LICENSE.txt;md5=c0e85c67b919e863a1a7a3da109dc40d"

PV = "20210119"
SRC_URI = "https://dl.google.com/coral/edgetpu_api/edgetpu_runtime_${PV}.zip"
SRC_URI[sha256sum] = "b23b2c5a227d7f0e65dcc91585028d27c12e764f8ce4c4db3f114be4a49af3ae"

S = "${UNPACKDIR}/edgetpu_runtime"

DEPENDS += "xz-native"
INSANE_SKIP:${PN} = "file-rdeps"

FILES:${PN} = "${libdir}/libedgetpu.so.*"

INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
INHIBIT_PACKAGE_STRIP = "1"

do_install() {
    install -d -m 0755 ${D}${libdir}
    install -m 0644 ${S}/${PN}/throttled/${HOST_ARCH}/libedgetpu.so.* ${D}${libdir}
}
