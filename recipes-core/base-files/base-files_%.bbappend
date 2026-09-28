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

# Scoped to i.MX6 so the other modules keep their task signatures.
do_install:append:mx6-nxp-bsp () {
    # NAND boots from UBI; GPT partition labels do not exist.
    if ${@ bb.utils.contains('MACHINE_FEATURES', 'nand', 'true', 'false', d)};then
        sed -i -e 's|^\(PART\)\?LABEL=rootfs[[:space:]].*|ubi0:rootfs\t/\t\tubifs\t\trw\t\t\t0\t0|' \
               -e '/^\(PART\)\?LABEL=boot[[:space:]]/d' ${D}${sysconfdir}/fstab
    fi
}

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
