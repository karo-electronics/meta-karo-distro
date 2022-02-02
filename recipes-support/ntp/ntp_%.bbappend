inherit relative_symlinks

PACKAGECONFIG_remove_karo-minimal = "openssl"

FILES_${PN}_remove = "${@ bb.utils.contains('DISTRO_FEATURES', 'dhcpcd', "${sysconfdir}/ntp.conf", "", d)}"

do_install_append() {
    if ${@ bb.utils.contains('DISTRO_FEATURES', 'dhcpcd', "true", "false", d)};then
        rm -f ${D}${sysconfdir}/ntp.conf
    fi
}
