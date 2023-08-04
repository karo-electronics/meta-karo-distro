inherit relative_symlinks
FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = "\
        file://rtcsymlink.sh \
        file://rtcsymlink.rule \
        ${@ bb.utils.contains('DISTRO_FEATURES', 'wayland', " file://weston.rule", "", d)} \
"
INITSCRIPT_PARAMS = "start 02 S ."

do_install:append() {
    install -v -m 0755 -d ${D}${nonarch_base_libdir}/udev
    install -v -m 0755 ${WORKDIR}/rtcsymlink.sh ${D}${nonarch_base_libdir}/udev/rtcsymlink.sh
    install -v -m 0755 ${WORKDIR}/rtcsymlink.rule ${D}${nonarch_base_libdir}/udev/rules.d/51-rtcsymlink.rules

    if ${@ bb.utils.contains('DISTRO_FEATURES', 'wayland', 'true', 'false', d)};then
        install -v -m 0755 ${WORKDIR}/weston.rule ${D}${nonarch_base_libdir}/udev/rules.d/50-weston.rules
    fi
}
