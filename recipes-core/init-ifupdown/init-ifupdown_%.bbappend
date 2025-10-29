FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

# default template file for /etc/interfaces.d/${iface}
SRC_URI:append = " file://iface"

# alternatively there may be individual template files for each listed NETWORK_INTERFACE
# with the same name as the network interface.
SRC_URI:append = " ${@ bb.utils.contains('MACHINE_FEATURES', 'stm-tsn-swch', " \
    file://br0 \
    file://eth0 \
    file://sw0ep \
    file://if-pre-up \
    file://if-up \
    file://if-down \
    file://if-post-down \
", "", d)}"

do_compile[noexec] := "1"

do_install:append () {
    rm -vf ${D}${sysconfdir}/network/if-pre-up.d/nfsroot
    install -v -d "${D}${sysconfdir}/network/interfaces.d"

    bbdebug 2 "interfaces='${NETWORK_INTERFACES}'"
    for iface in ${NETWORK_INTERFACES};do
        if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', "true", "false", d)}; then
            ifname=$(echo $iface | sed 's/eth/end/')
        else
            ifname=$iface
        fi
        if [ -s "${B}/${iface}" ];then
            sed "s/@@IFACE@@/${ifname}/g" "${B}/${iface}" > "${D}${sysconfdir}/network/interfaces.d/${ifname}"
        else
            sed "s/@@IFACE@@/${ifname}/g" "${B}/iface" > "${D}${sysconfdir}/network/interfaces.d/${ifname}"
        fi
        auto=false
        for a in ${NETWORK_INTERFACES_AUTO};do
            [ "$iface" = "$a" ] || [ "$ifname" = "$a" ] && auto=true && break
        done
        $auto || sed -i 's/^auto/#auto/' "${D}${sysconfdir}/network/interfaces.d/${ifname}"
    done
    if ${@bb.utils.contains('MACHINE_FEATURES', 'stm-tsn-swch', 'true', 'false', d)};then
        install -v -m 0755 ${B}/if-pre-up ${D}${sysconfdir}/network/if-pre-up.d/tsn-switch.sh
        install -v -m 0755 ${B}/if-up ${D}${sysconfdir}/network/if-up.d/tsn-switch.sh
        install -v -m 0755 ${B}/if-down ${D}${sysconfdir}/network/if-down.d/tsn-switch.sh
        install -v -m 0755 ${B}/if-post-down ${D}${sysconfdir}/network/if-post-down.d/tsn-switch.sh

        sed -i -e "s/^REF_ETH_INTERFACE=.*$/REF_ETH_INTERFACE=${DEFAULT_ETHERNET_MAIN_TSN_BRIDGE_INTERFACE}/" ${D}${sysconfdir}/network/if-pre-up.d/tsn-switch.sh
    fi
}
do_install[vardeps] += "NETWORK_INTERFACES NETWORK_INTERFACES_AUTO"
