FILESEXTRAPATHS:prepend := "${THISDIR}/splashscreen:"

FILES:${PN} += "/mnt/.psplash"

do_install:append() {
    install -d -m 0775 ${D}/mnt/.psplash
}

SPLASH_IMAGES = "file://karologo.png;outsuffix=default"
