ROOTFS_POSTPROCESS_COMMAND:append = " rootfs_postinst_cleanup"

rootfs_postinst_cleanup () {
set -vx
    # It's meaningless to rotate a log file, that is created once
    # upon boot in tmpfs
    rm -vf "${IMAGE_ROOTFS}${sysconfdir}/logrotate-dmesg.conf"

    # copy local timezone file from host, if none is installed
    if [ ! -s "${IMAGE_ROOTFS}${sysconfdir}/localtime" -a -s /etc/localtime ];then
        install -v /etc/localtime "${IMAGE_ROOTFS}${sysconfdir}"
    fi

    if ${@ bb.utils.contains('IMAGE_INSTALL','bash','true','false',d)};then
       	grep -q "^${base_bindir}/bash$" $D${sysconfdir}/shells || echo ${base_bindir}/bash >> $D${sysconfdir}/shells
    fi

    # remove unused file to prevent confusion
    rm -vf ${IMAGE_ROOTFS}${sysconfdir}/timezone

    if [ "${DISTRO}" = "karo-minimal" ];then
        rm -vrf ${IMAGE_ROOTFS}/usr/share/man
    fi

    if ${@ bb.utils.contains('DISTRO_FEATURES', 'reproducible-timestamps', "false", "true", d)};then
        date -u > ${IMAGE_ROOTFS}/.timestamp
    fi

    if ${@ bb.utils.contains('DISTRO_FEATURES', 'systemd', "false", "true", d)};then
        if [ -d "${IMAGE_ROOTFS}${sysconfdir}/default/volatiles" ];then
            bbwarn "${sysconfdir}/default/volatiles still exists"
        fi
    fi
}
