FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += "\
    file://adjtime \
    file://bash_aliases \
    file://bash_login \
"

dirs755:append = " \
    ${localstatedir}/lib/hwclock \
    ${prefix}/local \
"

do_install:append () {
    # Shell startup files for the root user; base-files owns ${ROOT_HOME}.
    install -v -d -m 0700 ${D}${ROOT_HOME}
    install -v -m 0644 ${UNPACKDIR}/bash_aliases ${D}${ROOT_HOME}/.bash_aliases
    install -v -m 0644 ${UNPACKDIR}/bash_login ${D}${ROOT_HOME}/.bash_login

    # Remove the dev note from the fstab; it is only relevant for developers
    # and must not end up in the target image.
    sed -i '/^# BEGIN-DEV-NOTE/,/^# END-DEV-NOTE/d' ${D}${sysconfdir}/fstab

    if ${@ bb.utils.contains('MACHINE_FEATURES', 'emmc', 'true', 'false', d)};then
        # enable fsck on the root filesystem (pass field 0 -> 1)
        sed -i '/root/s/0$/1/' ${D}${sysconfdir}/fstab
    fi

    if ${@ bb.utils.contains('DISTRO_FEATURES', 'read-only-rootfs', 'true', 'false', d)};then
        install -v -m 0644 ${UNPACKDIR}/adjtime ${D}${localstatedir}/lib/hwclock/adjtime
        ln -snvf ${localstatedir}/lib/hwclock/adjtime ${D}${sysconfdir}/adjtime
        # mount the root filesystem read-only
        sed -i '/root/s/defaults/ro/' ${D}${sysconfdir}/fstab
    else
        install -v -m 0644 ${UNPACKDIR}/adjtime ${D}${sysconfdir}/adjtime
    fi
}
