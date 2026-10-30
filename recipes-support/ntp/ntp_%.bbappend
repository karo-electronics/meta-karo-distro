# On dhcpcd systems the dhcpcd recipe owns /etc/ntp.conf (as a symlink
# into /run/dhcpcd, see its bbappend), so the ntp recipe must not ship
# its own copy.
FILES:${PN}:remove := "${@bb.utils.contains('DISTRO_FEATURES', 'dhcpcd', '${sysconfdir}/ntp.conf', '', d)}"

do_install:append() {
    if ${@bb.utils.contains('DISTRO_FEATURES', 'dhcpcd', 'true', 'false', d)};then
        rm -f ${D}${sysconfdir}/ntp.conf
    fi
}
