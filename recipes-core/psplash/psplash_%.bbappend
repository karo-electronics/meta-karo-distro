FILESEXTRAPATHS:prepend := "${THISDIR}/splashscreen:"

FILES:${PN} += "/mnt/.psplash"

do_install:append() {
    install -d -m 0775 ${D}/mnt/.psplash
}

SPLASH_IMAGES = "file://karologo.png;outsuffix=default"

# Single buffer only: DRM fbdev emulation cannot flip.
FILESEXTRAPATHS:prepend:mx6-nxp-bsp := "${THISDIR}/${PN}:"
SRC_URI:append:mx6-nxp-bsp = " file://0001-fb-never-request-double-buffering.patch"
