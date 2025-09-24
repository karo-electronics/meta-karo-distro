FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

do_install:append() {
    sed -i s/@@MACHINE@@/${MACHINE}/g ${D}${sysconfdir}/rauc/system.conf
}
