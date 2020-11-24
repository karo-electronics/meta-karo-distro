SUMMARY = "A qt5 image with Weston desktop for use with qt creator for app development"

require karo-image-weston.bb

inherit populate_sdk_qt5

IMAGE_INSTALL_append = " \
                       packagegroup-qt5 \
"

# qt creator needs openssh
IMAGE_FEATURES += "qtcreator-debug"

python extend_recipe_sysroot_append() {
    if d.getVar('DISTRO') != 'karo-wayland':
        raise_sanity_error("cannot build 'karo-image-qt5' with DISTRO '%s'" % d.getVar('DISTRO'), d)
}
