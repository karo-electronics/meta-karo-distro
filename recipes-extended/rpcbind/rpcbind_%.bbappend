# oe-core's rpcbind do_install:append unconditionally comments out the
# IPv6 Listen= lines in ${D}${systemd_system_unitdir}/*.socket when 'ipv6'
# is not in DISTRO_FEATURES. Without the 'systemd' PACKAGECONFIG the recipe
# configures with --without-systemdsystemunitdir, so no *.socket files are
# installed, the glob does not expand and the sed call fails. Install a
# placeholder socket so the glob always matches and remove it again
# afterwards.

do_install:prepend() {
    if ! ${@bb.utils.contains('DISTRO_FEATURES', 'ipv6', 'true', 'false', d)} \
       && ! ${@bb.utils.contains('PACKAGECONFIG', 'systemd', 'true', 'false', d)}; then
        install -d ${D}${systemd_system_unitdir}
        printf '' > ${D}${systemd_system_unitdir}/placeholder.socket
    fi
}

do_install:append() {
    rm -f ${D}${systemd_system_unitdir}/placeholder.socket
}
