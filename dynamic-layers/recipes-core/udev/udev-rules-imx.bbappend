FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = "\
    file://10-imx.rules \
"
do_install:prepend() {
    install -v -m 0755 -d ${D}${sysconfdir}/udev/rules.d
}
