inherit deploy

COMPATIBLE_MACHINE = "(mx8)"

FILESEXTRAPATHS:prepend := "${THISDIR}/uuu-templates:"
SRC_URI = " \
    file://uuu.auto.template \
"
LICENSE = "GPL-2.0-or-later"
LIC_FILES_CHKSUM = "file://uuu.auto.template;beginline=3;endline=8;md5=01388d9ecb5db377010590dc26f95680"

S = "${WORKDIR}"

FILES:${PN} = "uuu.auto.template"

do_install[noexec] = "1"

do_deploy () {
    install -vD ${S}/uuu.auto.template ${DEPLOYDIR}/uuu.auto.template
}
addtask deploy after do_compile
