FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

# dhcpcd discovers interfaces on its own (netlink); the udev
# integration would only add a build and runtime dependency on udev.
PACKAGECONFIG:remove = "udev"

FILES:${PN} += " \
    ${libdir}/dhcpcd \
    ${localstatedir}/lib/dhcpcd \
    ${sysconfdir}/ntp.conf \
    ${sysconfdir}/default/volatiles/99_dhcpcd \
"

SRC_URI:append = " \
    file://ntp.conf \
"

# /etc/ntp.conf is a symlink into the /run/dhcpcd/hook-state tree,
# which is seeded at boot by populate-volatile.sh from ${libdir}/dhcpcd.
# DHCP_SEED_INTERFACE is
# the name of the interface whose hook state provides the seed files:
# the configured primary interface, else the first automatic
# interface, else the first configured interface. The names in
# NETWORK_INTERFACES use the kernel 'eth*' scheme; the name as seen by
# the target (udev predictable naming) is derived with
# NETWORK_INTERFACE_PREFIX. Configuration mistakes are reported at
# parse time.
python () {
    import re

    interfaces = (d.getVar('NETWORK_INTERFACES') or '').split()
    auto = (d.getVar('NETWORK_INTERFACES_AUTO') or '').split()
    primary = d.getVar('PRIMARY_NETWORK_INTERFACE') or ''
    prefix = d.getVar('NETWORK_INTERFACE_PREFIX') or 'eth'

    def mapped(name):
        return re.sub('^eth', prefix, name)

    mapped_interfaces = [mapped(i) for i in interfaces]

    if primary and primary not in interfaces \
            and primary not in mapped_interfaces:
        bb.warn("dhcpcd: PRIMARY_NETWORK_INTERFACE '%s' is not listed "
                "in NETWORK_INTERFACES '%s'"
                % (primary, ' '.join(interfaces)))
    for a in auto:
        if a not in interfaces and a not in mapped_interfaces:
            bb.warn("dhcpcd: NETWORK_INTERFACES_AUTO entry '%s' is not "
                    "listed in NETWORK_INTERFACES '%s'"
                    % (a, ' '.join(interfaces)))

    if primary:
        seed = primary
    else:
        seed = ''
        for iface in interfaces:
            if iface in auto or mapped(iface) in auto:
                seed = iface
                break
        if not seed and interfaces:
            seed = interfaces[0]
    d.setVar('DHCP_SEED_INTERFACE', mapped(seed))
}

do_install:append () {
    sed -i 's/^duid/#duid/;s/^#clientid/clientid/' ${D}${sysconfdir}/dhcpcd.conf
    if ! grep -q '^quiet' ${D}${sysconfdir}/dhcpcd.conf;then
        cat << EOF >> ${D}${sysconfdir}/dhcpcd.conf

# Silence debug messages
quiet

# disable zeroconf IPs (169.254.1.0-169.254.254.255)
noarp
EOF
    fi

    install -v -d -m 0755 ${D}${libdir}/dhcpcd
    install -v -m 0644 ${UNPACKDIR}/ntp.conf ${D}${libdir}/dhcpcd/ntp.conf

    # The hook-state tree lives on the /run tmpfs; populate-volatile.sh
    # creates it at boot and seeds the config files from ${libdir}/dhcpcd
    install -d ${D}${sysconfdir}/default/volatiles
    cat << EOF > ${D}${sysconfdir}/default/volatiles/99_dhcpcd
d root root 0755 /run/dhcpcd none
d root root 0755 /run/dhcpcd/hook-state none
d root root 0755 /run/dhcpcd/hook-state/ntp.conf none
f root root 0644 /run/dhcpcd/hook-state/ntp.conf/${DHCP_SEED_INTERFACE}.dhcp ${libdir}/dhcpcd/ntp.conf
EOF
    ln -snvf /run/dhcpcd/hook-state/ntp.conf/${DHCP_SEED_INTERFACE}.dhcp ${D}${sysconfdir}/ntp.conf

    if ${@ bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
        # On systemd systems /etc/resolv.conf is owned by the systemd
        # recipe, which installs the resolv-conf.systemd alternative.
        # On sysvinit systems initscripts' 00_core volatiles owns
        # /etc/resolv.conf (symlink to /var/run/resolv.conf on the /run
        # tmpfs) and dhcpcd's 20-resolv.conf hook writes through it.
        ln -snvf resolv-conf.systemd ${D}${sysconfdir}/resolv.conf
    fi

    install -v -d -m 0755 ${D}${localstatedir}/lib/dhcpcd
    install -v -m 0644 /dev/null ${D}${localstatedir}/lib/dhcpcd/dhcpcd.duid
    ln -snvf ${localstatedir}/lib/dhcpcd/dhcpcd.duid ${D}${sysconfdir}/
}
