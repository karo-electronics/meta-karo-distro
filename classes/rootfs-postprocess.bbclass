ROOTFS_POSTPROCESS_COMMAND:append = " rootfs_postinst_cleanup"

rootfs_postinst_cleanup () {
    # It's meaningless to rotate a log file, that is created once
    # upon boot in tmpfs
    rm -vf "${IMAGE_ROOTFS}${sysconfdir}/logrotate-dmesg.conf"

    if [ "${DISTRO}" = "karo-minimal" ];then
        rm -vrf ${IMAGE_ROOTFS}/usr/share/man
    fi
}
