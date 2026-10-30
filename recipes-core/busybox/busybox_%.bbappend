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
# The Ka-Ro busybox configuration is expressed as a delta (karo.cfg)
# on top of the unmodified oe-core base defconfig, applied by
# busybox.inc's merge_config.sh invocation; there is no full defconfig
# shadowed here.
SRC_URI:append = " \
    file://karo.cfg \
    ${@ bb.utils.contains('DISTRO_FEATURES', 'pam', 'file://pam.cfg', '', d)} \
    ${@ bb.utils.contains('DISTRO_FEATURES', 'systemd', 'file://sysrq.conf', '', d)} \
    ${@ bb.utils.contains('TARGET_ARCH', 'x86_64', '', 'file://no-sha-hwaccel.cfg', d)} \
"

# The inetd/busybox-cron/busybox-httpd files shadowed through
# FILESEXTRAPATHS only add LSB init info headers, so update-rc.d
# derives the runlevel ordering from the scripts. Shadowing (instead
# of a patch) is the standard mechanism here: the files live in
# ${UNPACKDIR} outside the busybox source tree, which SRC_URI patches
# cannot reach.

PACKAGES =+ "${PN}-inetd"
FILES:${PN}-inetd = "${sysconfdir}/init.d/${PN}-inetd"
PROVIDES += "${PN}-inetd"

# busybox-inetd is recommended into every image with busybox, so the
# inetd super server starts on the sysvinit/mdev images. oe-core ships
# /etc/inetd.conf with every service commented out, so nothing is
# served by default; users enable services by editing the
# configuration.
INITSCRIPT_NAME:${PN}-inetd = "${PN}-inetd"
INITSCRIPT_PARAMS:${PN}-inetd = "start 02 2 3 4 5 . stop 01 0 1 6 ."
INITSCRIPT_PACKAGES += "${PN}-inetd"
RDEPENDS:${PN}-inetd = "busybox"
RRECOMMENDS:${PN} += "${INITSCRIPT_PACKAGES}"

# The utmp group is provided by the base-passwd recipe; /run/utmp is
# created with the right group at boot (initscripts bootmisc.sh with
# sysvinit, tmpfiles with systemd), so busybox itself needs neither
# the useradd class nor a base-passwd staging dependency.

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
                # Change the value of ENABLE_ROOTFS_FSCK in ${sysconfdir}/default/rcS to yes.
                # The sysvinit bbappend applies the same change to the rcS-default
                # file of the sysvinit recipe.
                sed -i '/^ENABLE_ROOTFS_FSCK=/s/no/yes/' ${D}${sysconfdir}/default/rcS
            fi
        fi
    else
        install -v -d 0755 ${D}${sysconfdir}/sysctl.d
        install -v -m 0664 ${UNPACKDIR}/sysrq.conf ${D}${sysconfdir}/sysctl.d/
    fi
}
