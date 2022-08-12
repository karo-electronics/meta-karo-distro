inherit deploy

COMPATIBLE_MACHINE = "(rzg2)"

FILESEXTRAPATHS:prepend := "${THISDIR}/templates:"
SRC_URI = " \
    file://install-bootloader.template \
    file://uuu.auto.template \
"
LICENSE = "CLOSED"

S = "${WORKDIR}"

FILES:${PN} = " \
    install-bootloader.template \
    uuu.auto.template \
"

do_install[noexec] = "1"

do_deploy () {
    install -vD ${S}/install-bootloader.template ${DEPLOYDIR}/install-bootloader.template
    install -vD ${S}/uuu.auto.template ${DEPLOYDIR}/uuu.auto.template
}
addtask deploy after do_compile
