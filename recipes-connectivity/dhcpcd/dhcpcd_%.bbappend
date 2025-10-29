FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

PACKAGECONFIG:remove = "udev"
FILES:${PN} += " \
            /run/dhcpcd/hook-state \
            ${localstatedir}/lib/dhcpcd \
            ${sysconfdir}/ntp.conf \
"

SRC_URI:append = " \
           file://ntp.conf \
           file://resolv.conf \
"

inherit relative_symlinks

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

    install -v -d -m 0755 ${D}/run/dhcpcd/hook-state

    IFNAME=""

    for iface in ${NETWORK_INTERFACES};do
        found=false
        if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', "true", "false", d)}; then
            ifname=$(echo $iface | sed 's/eth/end/')
        else
            ifname=$iface
        fi
        if [ -n "$IFNAME"];then
            IFNAME=$ifname
        fi
        if [ -n "${PRIMARY_NETWORK_INTERFACE}" ];then
            if [ "$iface" = "${PRIMARY_NETWORK_INTERFACE}" -o \
                 "$ifname" = "${PRIMARY_NETWORK_INTERFACE}" ];then
                IFNAME=$ifname
                found=true
                break
            fi
        fi
        for a in ${NETWORK_INTERFACES_AUTO};do
            if [ "$iface" = "$a" -o "$ifname" = "$a" ];then
                IFNAME=$ifname
                found=true
                break
            fi
        done
        $found && break
    done

    if ! $found && [ -n "${PRIMARY_NETWORK_INTERFACE}" ];then
        bbwarn "'${PRIMARY_NETWORK_INTERFACE}' is not listed in '\${NETWORK_INTERFACES}'"
    fi

    install -D -m 0644 ${WORKDIR}/ntp.conf ${D}/run/dhcpcd/hook-state/ntp.conf/${ifname}.dhcp
    ln -snvf /run/dhcpcd/hook-state/ntp.conf/${ifname}.dhcp ${D}${sysconfdir}/ntp.conf

    if ${@ bb.utils.contains('DISTRO_FEATURES', 'systemd', 'false', 'true', d)};then
        install -D -m 0644 ${WORKDIR}/resolv.conf ${D}/run/dhcpcd/hook-state/resolv.conf/${ifname}.dhcp
        ln -snvf /run/dhcpcd/hook-state/resolv.conf/${ifname}.dhcp ${D}${sysconfdir}/resolv.conf
    else
        ln -snvf resolv-conf.systemd ${D}${sysconfdir}/resolv.conf
    fi

    install -v -m 0755 -d ${D}${localstatedir}/lib/dhcpcd
    install -v -m 0755 /dev/null ${D}${localstatedir}/lib/dhcpcd/dhcpcd.duid
    ln -snvf ${localstatedir}/lib/dhcpcd/dhcpcd.duid ${D}${sysconfdir}/
}

# ignore empty dirs qa check
INSANE_SKIP:${PN} += "empty-dirs"
