FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

# default template file for /etc/interfaces.d/${iface}
SRC_URI:append = " file://iface"

do_install:append () {
    rm -vf ${D}${sysconfdir}/network/if-pre-up.d/nfsroot
    install -v -d "${D}${sysconfdir}/network/interfaces.d"

    bbdebug 2 "interfaces='${NETWORK_INTERFACES}'"
    for iface in ${NETWORK_INTERFACES};do
        if [ -s "${B}/${iface}" ];then
            sed "s/@@IFACE@@/${iface}/g" "${B}/${iface}" > "${D}${sysconfdir}/network/interfaces.d/${iface}"
        else
            sed "s/@@IFACE@@/${iface}/g" "${B}/iface" > "${D}${sysconfdir}/network/interfaces.d/${iface}"
        fi
        ifname=$(echo "$iface" | sed "s/^eth/${NETWORK_INTERFACE_PREFIX}/")
        auto=false
        for a in ${NETWORK_INTERFACES_AUTO};do
            [ "$iface" = "$a" ] || [ "$ifname" = "$a" ] && auto=true && break
        done
        $auto || sed -i 's/^auto/#auto/' "${D}${sysconfdir}/network/interfaces.d/${iface}"
    done
}
