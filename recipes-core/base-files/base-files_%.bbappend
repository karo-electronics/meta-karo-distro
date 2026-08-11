FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"
SRC_URI += "\
    file://adjtime \
"

dirs1777 = " \
    /tmp \
    ${localstatedir}/tmp \
"

dirs755:append = " \
    ${localstatedir}/log \
    ${localstatedir}/lib/hwclock \
    /run/lock \
    /run/network \
    ${prefix}/local \
"

require ${@ bb.utils.contains('DISTRO_FEATURES', 'systemd', '', 'no-systemd.inc', d)}

do_install:append () {
    if ${@ bb.utils.contains('MACHINE_FEATURES', 'emmc', 'true', 'false', d)};then
        sed -i '/root/s/0$/1/' ${D}${sysconfdir}/fstab
    fi
    if ${@ bb.utils.contains('IMAGE_FEATURES','read-only-rootfs','true','false',d)};then
        install -v -m 0744 ${WORKDIR}/adjtime ${D}${localstatedir}/lib/hwclock/adjtime
        ln -snvf ${localstatedir}/lib/hwclock/adjtime ${D}${sysconfdir}/adjtime
        sed -i '/root/s/rw/ro/' ${D}${sysconfdir}/fstab
    else
        install -v -m 0744 ${WORKDIR}/adjtime ${D}${sysconfdir}/adjtime
    fi
}
