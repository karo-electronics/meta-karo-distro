FILES:${PN}:remove = "${sysconfdir}/default/volatiles/99_lmbench"

do_install:append() {
    rm -rvf ${D}${sysconfdir}/default/volatiles
}
