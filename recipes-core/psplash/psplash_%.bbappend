FILESEXTRAPATHS:prepend := "${THISDIR}/splashscreen:"

FILES:${PN} += "/mnt/.psplash"

do_install:append() {
    install -d -m 0775 ${D}/mnt/.psplash
}

SPLASH_IMAGES = "file://karologo.png;outsuffix=default"

# Single buffer only: DRM fbdev emulation cannot flip.
FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"
SRC_URI += "file://0001-fb-never-request-double-buffering.patch"
