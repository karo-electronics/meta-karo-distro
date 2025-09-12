inherit relative_symlinks
FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = "\
        file://firmware.rules \
        file://rtcsymlink.sh \
        file://rtcsymlink.rule \
        ${@ bb.utils.contains('DISTRO_FEATURES', 'wayland', " file://weston.rule", "", d)} \
"
SRC_URI:append:stm32mp2 = " \
        ${@ bb.utils.contains('MACHINE_FEATURES', 'gpu', " file://galcore.rules", "", d)} \
"

do_install:append() {
    install -v -m 0755 -d ${D}${nonarch_base_libdir}/udev/rules.d
    install -v -m 0644 ${WORKDIR}/firmware.rules ${D}${nonarch_base_libdir}/udev/rules.d/50-firmware.rules
    install -v -m 0755 ${WORKDIR}/rtcsymlink.sh ${D}${nonarch_base_libdir}/udev/rtcsymlink.sh
    install -v -m 0644 ${WORKDIR}/rtcsymlink.rule ${D}${nonarch_base_libdir}/udev/rules.d/51-rtcsymlink.rules

    if ${@ bb.utils.contains('DISTRO_FEATURES', 'wayland', 'true', 'false', d)};then
        install -v -m 0644 ${WORKDIR}/weston.rule ${D}${nonarch_base_libdir}/udev/rules.d/50-weston.rules
    fi
}

do_install:append:stm32mp2() {
    if ${@ bb.utils.contains('MACHINE_FEATURES', 'gpu', 'true', 'false', d)};then
        install -v -m 0644 ${WORKDIR}/galcore.rules ${D}${nonarch_base_libdir}/udev/rules.d/72-galcore.rules
    fi
}
