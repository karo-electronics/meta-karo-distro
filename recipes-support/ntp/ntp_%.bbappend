inherit relative_symlinks useradd

PACKAGECONFIG_remove_karo-minimal = "openssl"

FILES_${PN}_remove = "${@ bb.utils.contains('DISTRO_FEATURES', 'dhcpcd', "${sysconfdir}/ntp.conf", "", d)}"

DEPENDS += "busybox"

USERADD_PACKAGES += "${PN}"
USERADD_PARAM_${PN} = "--system -U -d /nonexistent --no-create-home -s /bin/false ntp"
GROUPADD_PARAM_${PN} = "-f --system crontab"

do_install_append() {
    if ${@ bb.utils.contains('DISTRO_FEATURES', 'dhcpcd', "true", "false", d)};then
        rm -f ${D}${sysconfdir}/ntp.conf
    fi
}
