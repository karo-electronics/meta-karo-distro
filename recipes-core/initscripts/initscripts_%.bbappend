FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

# bootmisc.sh, checkroot.sh, mountall.sh etc. are Ka-Ro modified copies
# that shadow the upstream initscripts files through FILESEXTRAPATHS
# (no SRC_URI entry needed); umountroot and vars.sh are Ka-Ro specific
# and must be added explicitly. init_files lists the scripts that carry
# the @@LIBDIR@@ placeholder for the sed substitution in do_install.
init_files = "bootmisc.sh umountroot"
SRC_URI:append = " file://umountroot file://vars.sh"

FILES:${PN} += "${libdir}/init/vars.sh"
CONFFILES:${PN} += "${sysconfdir}/init.d/umountroot"

do_install:append () {
    install -d -m 0755 ${D}${libdir}/init
    # The Ka-Ro images do not ship logrotate, so the rotation config for
    # the dmesg boot log would be inert; drop it at the source
    rm -f ${D}${sysconfdir}/logrotate-dmesg.conf
    install -m 0755 ${UNPACKDIR}/vars.sh ${D}${libdir}/init/vars.sh

    install -m 0755 ${UNPACKDIR}/umountroot ${D}${sysconfdir}/init.d/umountroot
    update-rc.d -r ${D} -v umountroot start 41 0 6 .

    install -m 0755 ${UNPACKDIR}/checkfs.sh ${D}${sysconfdir}/init.d/checkfs.sh
    update-rc.d -r ${D} -v checkfs.sh start 04 S .

    update-rc.d -r ${D} -v -f checkroot.sh remove
    update-rc.d -r ${D} -v -f mountall.sh remove

    update-rc.d -r ${D} -v checkroot.sh start 03 S .
    update-rc.d -r ${D} -v mountall.sh start 05 S .

    for f in ${init_files};do
        sed -i "s:@@LIBDIR@@:${libdir}:g" "${D}${sysconfdir}/init.d/$f"
    done
}
