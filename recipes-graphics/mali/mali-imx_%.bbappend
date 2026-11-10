FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

# SysVinit support for dual-driver mode. The upstream recipe only
# inherits systemd and installs mali-imx.service; on sysvinit images
# (no 'systemd' in DISTRO_FEATURES) the systemd class is a no-op, so
# apply the same boot-time driver configuration through an init.d
# script running the installed ${libexecdir}/mali-imx-boot instead.
SRC_URI:append = " file://mali-imx.init"

inherit_defer ${@bb.utils.contains('IMX_MALI_DUAL_DRIVER', '1', 'update-rc.d', '', d)}

INITSCRIPT_PACKAGES = "${PN}-dual"
INITSCRIPT_NAME = "mali-imx"
# Run in runlevel S right after mountall.sh (start 05 S), i.e. after
# the local filesystems are up but before the display manager/Weston
# spawned from the inittab respawn entries - the sysvinit counterpart
# of the unit's After=local-fs.target / Before=weston.service.
INITSCRIPT_PARAMS = "start 06 S ."

FILES:${PN}-dual += "${sysconfdir}/init.d/mali-imx"
FILES:${PN} += "${nonarch_base_libdir}/firmware/mali_csffw.bin"

do_install:append() {
    if [ "${IMX_MALI_DUAL_DRIVER}" = "1" ]; then
        install -d ${D}${sysconfdir}/init.d
        install -m 0755 ${UNPACKDIR}/mali-imx.init ${D}${sysconfdir}/init.d/mali-imx
    fi
    # kbase loads "mali_csffw.bin" from the firmware root; linux-firmware ships it under arm/mali/arch10.12/
    install -d ${D}${nonarch_base_libdir}/firmware
    ln -sf arm/mali/arch10.12/mali_csffw.bin ${D}${nonarch_base_libdir}/firmware/mali_csffw.bin
}
