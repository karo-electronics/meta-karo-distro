inherit relative_symlinks

PACKAGECONFIG_remove_karo-minimal = "openssl"

FILES_${PN}_remove = "${sysconfdir}/ntp.conf"

do_install_append() {
    rm -f ${D}${sysconfdir}/ntp.conf
}
