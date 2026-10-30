FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

# bootmisc.sh, checkroot.sh, mountall.sh, reboot and umountfs are
# Ka-Ro copies that shadow the upstream initscripts files through
# FILESEXTRAPATHS (no SRC_URI entry needed); umountroot and vars.sh
# are Ka-Ro specific and must be added explicitly. bootmisc.sh and
# umountroot source vars.sh from its install location,
# /usr/lib/init/vars.sh (the Debian convention for this helper).
#
# Deltas that were carried by older Ka-Ro copies and turned out to be
# unnecessary are dropped in favor of the upstream files:
#  - functions: the removed log_success_msg/log_failure_msg/
#    log_warning_msg helpers are unused; the upstream file is a
#    superset and works as-is.
#  - devpts: whitespace-only difference.
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
}
