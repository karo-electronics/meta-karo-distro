FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

PACKAGECONFIG:append:stm32mp2 = "${@ bb.utils.contains('MACHINE_FEATURES', 'gpu', "", " use-pixman", d)}"

do_install:prepend() {
    # Replace template variables
    sed -i -e 's,@bindir@,${bindir},g' ${WORKDIR}/weston.ini
}
