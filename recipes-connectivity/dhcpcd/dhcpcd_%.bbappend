FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

PACKAGECONFIG:remove = "udev"
FILES:${PN} += " \
            ${libdir}/dhcpcd \
            ${localstatedir}/lib/dhcpcd \
            ${sysconfdir}/ntp.conf \
            ${sysconfdir}/default/volatiles/99_dhcpcd \
"

SRC_URI:append = " \
           file://ntp.conf \
           file://resolv.conf \
"

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

    for iface in ${NETWORK_INTERFACES};do
        found=false
        ifname=$(echo "$iface" | sed "s/^eth/${KARO_NET_IFPREFIX}/")
        if [ -n "${PRIMARY_NETWORK_INTERFACE}" ];then
            if [ "$iface" = "${PRIMARY_NETWORK_INTERFACE}" -o \
                 "$ifname" = "${PRIMARY_NETWORK_INTERFACE}" ];then
                found=true
                break
            fi
        fi
        $found && break
    done
    if ! $found && [ -n "${PRIMARY_NETWORK_INTERFACE}" ];then
        bbwarn "'${PRIMARY_NETWORK_INTERFACE}' is not listed in 'NETWORK_INTERFACES'"
        ifname=$(echo "${PRIMARY_NETWORK_INTERFACE}" | sed "s/^eth/${KARO_NET_IFPREFIX}/")
    fi
    for iface in ${NETWORK_INTERFACES};do
        found=false
        ifname=$(echo "$iface" | sed "s/^eth/${KARO_NET_IFPREFIX}/")
        for a in ${NETWORK_INTERFACES_AUTO};do
            if [ "$iface" = "$a" -o "$ifname" = "$a" ];then
                found=true
                break
            fi
        done
        $found && break
    done
    if ! $found && [ -n "${NETWORK_INTERFACES_AUTO}" ];then
        bbwarn "'${NETWORK_INTERFACES_AUTO}' is not listed in 'NETWORK_INTERFACES'"
    fi

    # The hook-state tree lives on the /run tmpfs; populate-volatile.sh
    # creates it at boot and seeds the config files from ${libdir}/dhcpcd
    install -d ${D}${sysconfdir}/default/volatiles
    cat << EOF > ${D}${sysconfdir}/default/volatiles/99_dhcpcd
d root root 0755 /run/dhcpcd none
d root root 0755 /run/dhcpcd/hook-state none
d root root 0755 /run/dhcpcd/hook-state/ntp.conf none
f root root 0644 /run/dhcpcd/hook-state/ntp.conf/${ifname}.dhcp ${libdir}/dhcpcd/ntp.conf
EOF
    ln -snvf /run/dhcpcd/hook-state/ntp.conf/${ifname}.dhcp ${D}${sysconfdir}/ntp.conf

    if ${@ bb.utils.contains('DISTRO_FEATURES', 'systemd', 'false', 'true', d)};then
        install -v -m 0644 ${UNPACKDIR}/resolv.conf ${D}${libdir}/dhcpcd/resolv.conf
        cat << EOF >> ${D}${sysconfdir}/default/volatiles/99_dhcpcd
d root root 0755 /run/dhcpcd/hook-state/resolv.conf none
f root root 0644 /run/dhcpcd/hook-state/resolv.conf/${ifname}.dhcp ${libdir}/dhcpcd/resolv.conf
EOF
        ln -snvf /run/dhcpcd/hook-state/resolv.conf/${ifname}.dhcp ${D}${sysconfdir}/resolv.conf
    else
        ln -snvf resolv-conf.systemd ${D}${sysconfdir}/resolv.conf
    fi

    install -v -m 0755 -d ${D}${localstatedir}/lib/dhcpcd
    install -v -m 0755 /dev/null ${D}${localstatedir}/lib/dhcpcd/dhcpcd.duid
    ln -snvf ${localstatedir}/lib/dhcpcd/dhcpcd.duid ${D}${sysconfdir}/
}
