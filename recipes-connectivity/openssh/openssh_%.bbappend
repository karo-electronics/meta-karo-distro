inherit relative_symlinks

RRECOMMENDS:${PN}-sshd:remove:class-target = " rng-tools"
RRECOMMENDS:${PN}-sshd:append:class-target = " haveged"

FILES:${PN}:remove = "${sysconfdir}/default/volatiles/99_sshd"

do_install:append() {
    rm -rvf ${D}${sysconfdir}/default/volatiles
}
