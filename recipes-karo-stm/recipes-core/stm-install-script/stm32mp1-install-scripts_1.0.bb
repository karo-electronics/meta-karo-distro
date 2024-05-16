inherit deploy

COMPATIBLE_MACHINE = "(stm32mp1)"

PROVIDES += "uuu-script-template"

FILESEXTRAPATHS:prepend := "${THISDIR}/templates:"
SRC_URI = " \
    file://uuu.auto.template \
    file://start-fastboot.template \
"

LICENSE = "CLOSED"

S = "${WORKDIR}"

FILES:${PN} = " \
    uuu.auto.template \
    start-fastboot.template \
"

do_install[noexec] = "1"

do_deploy () {
    install -vD ${S}/uuu.auto.template -t ${DEPLOYDIR}
    install -vD ${S}/start-fastboot.template -t ${DEPLOYDIR}
}
addtask deploy after do_compile
