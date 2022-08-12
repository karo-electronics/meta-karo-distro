FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"
do_install:append () {
	rm ${D}${sysconfdir}/network/if-pre-up.d/nfsroot
}
