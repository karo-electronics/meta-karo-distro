inherit relative_symlinks

FILES:${PN}:remove = "${sysconfdir}/default/volatiles/volatiles.04_pulse"

do_install:append() {
    rm -rvf ${D}${sysconfdir}/default/volatiles
}
