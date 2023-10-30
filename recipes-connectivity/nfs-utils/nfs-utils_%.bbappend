PACKAGES:remove = "${PN}-stats"

INITSCRIPT_PACKAGES:remove = "${@ bb.utils.contains('DISTRO_FEATURES', 'nfs-server', "", "${PN}", d)}"

do_install:append() {
    for f in ${FILES:nfs-utils-stats};do
	rm -vf ${D}${f}
    done
    if ${@ bb.utils.contains('DISTRO_FEATURES', 'nfs-server', 'false', 'true', d)};then
        rm -vf ${D}${sysconfdir}/init.d/nfsserver
    fi
}
