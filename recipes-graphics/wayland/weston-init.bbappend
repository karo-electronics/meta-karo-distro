do_install:append() {
    # The sysvinit startup path runs Weston through weston-start as the
    # unprivileged 'weston' user. meta-imx-bsp's weston-start passes
    # --socket=/run/wayland-0, but /run is a root-owned 0755 tmpfs on
    # sysvinit images, so Weston exits with "fatal: failed to add socket:
    # Permission denied" right after opening the seat. Use the per-user
    # socket in $XDG_RUNTIME_DIR (which weston-start creates 0700 for the
    # weston user) instead. Under systemd, weston-start is not installed
    # at all: Weston runs from weston.service on the weston.socket unit.
    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'false', 'true', d)};then
        sed -i -e 's| --socket=/run/wayland-0||' ${D}${bindir}/weston-start
    fi

    # NXP's imx-nxp-bsp/weston.ini shadows the upstream file via the
    # FILESPATH override directories and leaves require-input at its
    # default (true), so Weston would exit when no input devices are
    # attached.
    sed -i -e '/^\[core\]/a require-input=false' ${D}${sysconfdir}/xdg/weston/weston.ini

    # Load the Adwaita cursors shipped by adwaita-icon-theme-cursors in
    # karo-image-weston; without a cursor theme Weston logs
    # "could not load cursor 'dnd-move'" etc. and shows no pointer.
    sed -i -e '/^\[shell\]/a cursor-theme=Adwaita' ${D}${sysconfdir}/xdg/weston/weston.ini
}
