FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

# default template file for /etc/interfaces.d/${iface}
SRC_URI:append = " file://iface"
# alternatively there may be individual template files for each listed NETWORK_INTERFACE
# with the same name as the network interface.

do_install:append () {
    rm -v ${D}${sysconfdir}/network/if-pre-up.d/nfsroot

    for iface in ${NETWORK_INTERFACES};do
        install -v -d "${D}${sysconfdir}/network/interfaces.d"
        if [ -s "${B}/${iface}" ];then
            sed "s/@@IFACE@@/${iface}/g" "${B}/${iface}" > "${D}${sysconfdir}/network/interfaces.d/${iface}"
        else
            sed "s/@@IFACE@@/${iface}/g" "${B}/iface" > "${D}${sysconfdir}/network/interfaces.d/${iface}"
        fi
    done
}
do_install[vardeps] += "NETWORK_INTERFACES"
