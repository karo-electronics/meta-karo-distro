FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:${THISDIR}/files:"

# Drop config fragments from other layers that re-enable unwanted
# utilities: ftpget.cfg (meta-imx, ftpget/ftpput applets), and the
# oe-core fragments for the busybox user management utilities
# (adduser/addgroup/..., handled by shadow in the Ka-Ro images), the
# resize applet and the sha1sum fragment (SHA hardware acceleration
# is governed by no-sha-hwaccel.cfg instead).
SRC_URI:remove = " \
    file://ftpget.cfg \
    file://login-utilities.cfg \
    file://resize.cfg \
    file://sha1sum.cfg \
"
SRC_URI:append = " \
    ${@ bb.utils.contains('DISTRO_FEATURES', 'pam', 'file://pam.cfg', '', d)} \
    ${@ bb.utils.contains('DISTRO_FEATURES', 'systemd', 'file://sysrq.conf', '', d)} \
    ${@ bb.utils.contains('TARGET_ARCH', 'x86_64', '', 'file://no-sha-hwaccel.cfg', d)} \
"

PACKAGES =+ "${PN}-inetd"
FILES:${PN}-inetd = "${sysconfdir}/init.d/${PN}-inetd"
PROVIDES += "${PN}-inetd"

INITSCRIPT_NAME:${PN}-inetd = "${PN}-inetd"
INITSCRIPT_PARAMS:${PN}-inetd = "start 02 2 3 4 5 . stop 01 0 1 6 ."
INITSCRIPT_PACKAGES += "${PN}-inetd"
RDEPENDS:${PN}-inetd = "busybox"
RRECOMMENDS:${PN} += "${INITSCRIPT_PACKAGES}"

inherit useradd

USERADD_PACKAGES += "${PN}"
USERADD_PARAM:${PN} = ""
GROUPADD_PARAM:${PN} = "--system utmp"

# need /etc/group in the staging directory
DEPENDS += "base-passwd"

# when pam is selected in the DISTRO_FEATURES the target-libpam must be installed
# for the busybox build to work
DEPENDS += "${@ bb.utils.contains('DISTRO_FEATURES','pam','libpam','',d)}"

FILES:${PN} += "${@ bb.utils.contains('DISTRO_FEATURES', 'systemd', \
                    "${sysconfdir}/sysctl.d/sysrq.conf", \
                    "${sysconfdir}/network/run ${sysconfdir}/default/volatiles/99_busybox", d)}"

do_install:append () {
    if ${@ bb.utils.contains('DISTRO_FEATURES', 'systemd', 'false', 'true', d)};then
        if [ -e ${D}${sysconfdir}/init.d/inetd.${BPN} ];then
            mv -vi ${D}${sysconfdir}/init.d/inetd.${BPN} ${D}${sysconfdir}/init.d/${PN}-inetd
        fi

        install -d -m 0755 ${D}${sysconfdir}/network
        ln -snvf /run/network ${D}${sysconfdir}/network/run

        # ifup/udhcpc state directory on the /run tmpfs, created at boot
        install -d ${D}${sysconfdir}/default/volatiles
        printf 'd root root 0755 /run/network none\n' \
            > ${D}${sysconfdir}/default/volatiles/99_busybox

        if ${@ bb.utils.contains('MACHINE_FEATURES', 'emmc', 'true', 'false', d)};then
            if grep -q "CONFIG_INIT=y" ${B}/.config && \
                    ${@bb.utils.contains('VIRTUAL-RUNTIME_init_manager','mdev-busybox','true','false',d)}; then
                # Change the value of ENABLE_ROOTFS_FSCK in ${sysconfdir}/default/rcS to yes
                sed -i '/^ENABLE_ROOTFS_FSCK=/s/no/yes/' ${D}${sysconfdir}/default/rcS
            fi
        fi
    else
        install -v -d 0755 ${D}${sysconfdir}/sysctl.d
        install -v -m 0664 ${UNPACKDIR}/sysrq.conf ${D}${sysconfdir}/sysctl.d/
    fi
}
