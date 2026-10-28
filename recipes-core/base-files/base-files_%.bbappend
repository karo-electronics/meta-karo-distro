FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += "\
    file://adjtime \
"

dirs755:append = " \
    ${localstatedir}/lib/hwclock \
    ${prefix}/local \
"

do_install:append () {
    # Remove the dev note from the fstab; it is only relevant for developers
    # and must not end up in the target image.
    sed -i '/^# BEGIN-DEV-NOTE/,/^# END-DEV-NOTE/d' ${D}${sysconfdir}/fstab
    if ${@ bb.utils.contains('MACHINE_FEATURES', 'emmc', 'true', 'false', d)};then
        sed -i '/root/s/0$/1/' ${D}${sysconfdir}/fstab
    fi
    if [ "${KARO_ROOTFS_READ_ONLY}" = "1" ];then
        install -v -m 0744 ${UNPACKDIR}/adjtime ${D}${localstatedir}/lib/hwclock/adjtime
        ln -snvf ${localstatedir}/lib/hwclock/adjtime ${D}${sysconfdir}/adjtime
        sed -i '/root/s/rw/ro/' ${D}${sysconfdir}/fstab
    else
        install -v -m 0744 ${UNPACKDIR}/adjtime ${D}${sysconfdir}/adjtime
    fi
}
