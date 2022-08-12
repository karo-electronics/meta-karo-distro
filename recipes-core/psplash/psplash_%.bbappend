FILES:${PN} += "/mnt/.psplash"

do_install:append() {
    install -d -m 0775 ${D}/mnt/.psplash
}
