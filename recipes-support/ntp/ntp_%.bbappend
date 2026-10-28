remove_ntp_conf = "${@ bb.utils.contains('DISTRO_FEATURES', 'dhcpcd', True, False, d)}"

FILES:${PN}:remove := "${@ "${sysconfdir}/ntp.conf" if ${remove_ntp_conf} else ""}"

do_install:append() {
    if [ "${remove_ntp_conf}" = "True" ];then
        rm -f ${D}${sysconfdir}/ntp.conf
    fi
}
