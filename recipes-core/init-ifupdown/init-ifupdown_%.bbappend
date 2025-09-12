FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

# default template file for /etc/interfaces.d/${iface}
SRC_URI:append = " file://iface"
# alternatively there may be individual template files for each listed NETWORK_INTERFACE
# with the same name as the network interface.

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
}
do_install[vardeps] += "NETWORK_INTERFACES NETWORK_INTERFACES_AUTO"
