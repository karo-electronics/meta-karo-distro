inherit deploy

COMPATIBLE_MACHINE = "(mx8)"

FILESEXTRAPATHS_prepend := "${THISDIR}/uuu-templates:"
SRC_URI = " \
    file://uuu.auto.template \
"

LICENSE = "CLOSED"

S = "${WORKDIR}"

FILES_${PN} = "uuu.auto.template"

do_install[noexec] = "1"

do_deploy() {
    install -vD ${S}/uuu.auto.template ${DEPLOYDIR}/uuu.auto.template
}
addtask deploy after do_compile
