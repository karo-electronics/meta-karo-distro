FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:remove = "\
    file://72-switch-ep-static-ip.network \
    file://72-switch-ep-dhcp.network.sample \
"
SRC_URI:append = " \
    file://72-switch-ep-dhcp.network \
    file://72-switch-ep-static-ip.network.sample \
"

do_install() {
    if ${@bb.utils.contains('DISTRO_FEATURES','systemd','true','false',d)}; then
        install -d ${D}${sbindir}
        install -m 0755 ${WORKDIR}/ttt-ip-common.sh ${D}${sbindir}
        sed -i -e "s/^REF_ETH_INTERFACE=.*$/REF_ETH_INTERFACE=${DEFAULT_ETHERNET_MAIN_TSN_BRIDGE_INTERFACE}/" ${D}${sbindir}/ttt-ip-common.sh

        install -d ${D}${systemd_unitdir}/system ${D}${systemd_unitdir}/network
        install -m 0644 ${WORKDIR}/72-br0.netdev ${D}${systemd_unitdir}/network
        install -m 0644 ${WORKDIR}/72-br0.network ${D}${systemd_unitdir}/network
        install -m 0644 ${WORKDIR}/72-switch.network ${D}${systemd_unitdir}/network
        install -m 0644 ${WORKDIR}/72-switch-ep-dhcp.network ${D}${systemd_unitdir}/network

        install -d ${D}${systemd_unitdir}/system
        install -m 0644 ${WORKDIR}/st-init-switch.service ${D}${systemd_unitdir}/system/
    fi
}
