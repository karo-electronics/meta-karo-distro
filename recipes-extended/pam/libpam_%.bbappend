do_install:append() {
    rm -rvf ${D}${sysconfdir}/default/volatiles
}
