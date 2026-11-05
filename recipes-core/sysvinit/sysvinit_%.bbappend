FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

do_install:append () {
    if ${@ bb.utils.contains('MACHINE_FEATURES', 'emmc', 'true', 'false', d)} && \
            ${@bb.utils.contains('VIRTUAL-RUNTIME_init_manager','sysvinit','true','false',d)}; then
        # Change the value of ENABLE_ROOTFS_FSCK in ${sysconfdir}/default/rcS
        # to yes. The busybox bbappend applies the same change to the rcS.default
        # file of the busybox recipe (mdev-busybox init).
        sed -i '/^ENABLE_ROOTFS_FSCK=/s/no/yes/' ${D}${sysconfdir}/default/rcS
    fi
}
