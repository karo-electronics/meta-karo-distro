inherit deploy

COMPATIBLE_MACHINE = "(stm32mp1)"

FILESEXTRAPATHS:prepend := "${THISDIR}/templates:"
SRC_URI = " \
    file://uuu.auto.template \
"
LICENSE = "CLOSED"

S = "${WORKDIR}"

FILES:${PN} = " \
    uuu.auto.template \
"

do_install[noexec] = "1"

do_deploy () {
    install -vD ${S}/uuu.auto.template ${DEPLOYDIR}/uuu.auto.template
}
addtask deploy after do_compile
