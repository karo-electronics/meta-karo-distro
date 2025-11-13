IMAGE_LINGUAS = ""

IMAGE_FEATURES:remove = "tools-debug"

# remove packages that pull in openssl
IMAGE_INSTALL:remove = "curl git"

ROOTFS_PARTITION_SIZE:stm32mp1 ?= "${@bb.utils.contains('MACHINE_FEATURES',"nand", \
                          64*1024,bb.utils.contains('DISTRO_FEATURES', "rauc", \
                          256*1024, 128*1024, d), d)}"

ROOTFS_PARTITION_SIZE:stm32mp2 ?= "${@bb.utils.contains('MACHINE_FEATURES',"nand", \
                          64*1024,bb.utils.contains('DISTRO_FEATURES', "rauc", \
                          256*1024, bb.utils.contains('DISTRO_FEATURES', "systemd", \
                          150*1024, bb.utils.contains('DISTRO_FEATURES', "bluetooth", \
                          140*1024, 128*1024, d), d), d), d)}"

UBI_VOLNAME = "rootfs"

export UBIVOL_FILES_rootfs = "${IMAGE_ROOTFS}"
