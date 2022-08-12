FILES:${PN} += "/run/dbus"

do_install:append:class-target() {
    rm -rvf ${D}${sysconfdir}/default/volatiles
    install -v -d -o messagebus -g messagebus -m 0755 ${D}/run/dbus
}

# ignore empty dirs qa check
INSANE_SKIP:${PN} += "empty-dirs"
