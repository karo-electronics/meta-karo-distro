SUMMARY = "A qt5 image with Weston desktop for use with qt creator for app development"

require karo-image-weston.bb

inherit populate_sdk_qt5

IMAGE_INSTALL_append = " \
                       packagegroup-qt5 \
"

# qt creator needs openssh
IMAGE_FEATURES += "qtcreator-debug"

ROOTFS_PARTITION_SIZE = "2097152"
