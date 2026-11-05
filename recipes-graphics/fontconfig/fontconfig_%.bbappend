FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = " \
    file://99-writable-cache.conf \
"

do_install:append() {
    # The rootfs is mounted read-only (read-only-rootfs), so
    # /var/cache/fontconfig and $HOME/.cache are not writable at runtime
    # and fontconfig spams "No writable cache directories". /tmp is a
    # writable tmpfs (/var/volatile/tmp), so add it as a cache dir.
    install -D -m 0644 ${UNPACKDIR}/99-writable-cache.conf ${D}${sysconfdir}/fonts/conf.d/99-writable-cache.conf
}
