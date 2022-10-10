DESCRIPTION = "Package group for Qt6"

inherit packagegroup

# Install fonts
QT6_FONTS ?= "ttf-dejavu-common ttf-dejavu-sans ttf-dejavu-sans-mono ttf-dejavu-serif"

# Install qtquick3d
QT6_QTQUICK3D ?= "qtquick3d"

QT6_IMAGE_INSTALL = " \
    ${QT6_FONTS} \
    ${QT6_QTQUICK3D} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'x11', 'libxkbcommon', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'wayland', 'qtwayland qtwayland-plugins', '', d)} \
    packagegroup-qt6-modules \
    qtbase \
    qtbase-plugins \
    qtmultimedia \
"

IMAGE_INSTALL += " \
    ${QT6_FONTS} \ 
"

RDEPENDS:${PN} += " \
    ${QT6_IMAGE_INSTALL} \
"
