PACKAGES =+ "util-linux-setterm"

ALTERNATIVE:util-linux-setterm += "setterm"
ALTERNATIVE_LINK_NAME[setterm] = "${bindir}/setterm"
FILES:util-linux-setterm = "${bindir}/setterm*"

do_install:append() {
    echo 'MOUNTALL="-t nonfs,smbfs,ncpfs"' > ${D}${sysconfdir}/default/mountall
}
