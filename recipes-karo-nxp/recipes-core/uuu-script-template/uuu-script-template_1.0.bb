inherit deploy

COMPATIBLE_MACHINE = "(mx8|mx93)"

FILESEXTRAPATHS:prepend := "${THISDIR}/uuu-templates:"
SRC_URI = " \
    file://uuu.auto.template \
"
LICENSE = "GPL-2.0-or-later"
LIC_FILES_CHKSUM = "file://uuu.auto.template;beginline=3;endline=7;md5=d1866cc8881b6a65ff00c310746803a8"
LIC_FILES_CHKSUM:mx9-nxp-bsp = "file://uuu.auto.template;beginline=3;endline=7;md5=5e506a12102aedbe43d515374701910b"

S = "${WORKDIR}"

FILES:${PN} = "uuu.auto.template"

do_install[noexec] = "1"

do_deploy () {
    install -vD ${S}/uuu.auto.template ${DEPLOYDIR}/uuu.auto.template
}
addtask deploy after do_compile
