FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

do_install:prepend() {
    # Replace template variables
    sed -i -e 's,@bindir@,${bindir},g' ${UNPACKDIR}/weston.ini
}
