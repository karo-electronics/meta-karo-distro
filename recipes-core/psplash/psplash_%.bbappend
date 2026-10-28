FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SPLASH_IMAGES = "file://karologo.png;outsuffix=default"

# Single buffer only: DRM fbdev emulation cannot flip.
SRC_URI:append = " file://0001-fb-never-request-double-buffering.patch"
