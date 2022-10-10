FILES:${PN} += "/run/wpa_supplicant"

do_install:append() {
    rm -rvf ${D}${sysconfdir}/default/volatiles
    install -v -d -m 0700 ${D}/run/wpa_supplicant
}
INSANE_SKIP:${PN} += "empty-dirs"
