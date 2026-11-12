LICENSE = "MIT"

inherit core-image features_check rootfs-postprocess

IMAGE_FSTYPES:append = " ${@bb.utils.contains('MACHINE_FEATURES', 'nand', 'ubi', 'wic.zst', d)}"

require ${@ "karo-wic.inc" if any(fstype.startswith('wic') for fstype in d.getVar('IMAGE_FSTYPES').split()) else ""}

require ${@ "karo-ubi.inc" if "ubi" in d.getVar('IMAGE_FSTYPES').split() else ""}

# Vendor specific flash package generation for NXP, RZG2 and STM32MP-based machines
KARO_PACKAGING_VENDOR:use-nxp-bsp = "nxp"
KARO_PACKAGING_VENDOR:rzg2 = "rzg2"
KARO_PACKAGING_VENDOR:stm32mpcommon = "stm"

require packaging/karo-${KARO_PACKAGING_VENDOR}.inc

inherit extrausers
EXTRA_USERS_PARAMS ?= "usermod -p '' root"

REQUIRED_DISTRO_FEATURES ??= ""

ROOTFS_POSTPROCESS_COMMAND:remove = "\
    rootfs_update_timestamp \
    ${@bb.utils.contains('IMAGE_FEATURES','read-only-rootfs','empty_var_volatile','',d)} \
"

# Default partition sizes (KiB)
BOOTFS_PARTITION_SIZE ??= "65536"
ROOTFS_PARTITION_SIZE ??= "131072"
USERFS_PARTITION_SIZE ??= "131072"

DEPENDS:append = " cross-localedef-native qemuwrapper-cross"

# First-boot auto-resize of the userfs partition into the remaining
# storage space. Enabled by default, except when rauc is used.
KARO_RESIZE_USERFS ??= "${@ bb.utils.contains('DISTRO_FEATURES', 'rauc', '0', '1', d)}"

# Kernel image and devicetree files are provided by the dedicated
# boot partition, so don't install them into the rootfs images.
BAD_RECOMMENDATIONS += "kernel-image* kernel-devicetree"

PREFERRED_PROVIDER_fdisk = "util-linux"

PACKAGE_INSTALL += "${@ bb.utils.contains('DISTRO_FEATURES', 'rauc', "u-boot-script", "", d)}"

IMAGE_FEATURES:append = " tools-debug"

DEBUG_TOOLS ?= " \
    gdbserver \
    i2c-tools \
    libgpiod \
    libgpiod-tools \
    spidev-test \
    strace \
    tcpdump \
"

IMAGE_INSTALL:append = " \
    ${@bb.utils.contains('DISTRO_FEATURES','alsa', \
        'alsa-utils-aplay alsa-utils-amixer alsa-utils-alsamixer','',d)} \
    bash \
    canutils \
    coreutils \
    ${@bb.utils.contains('DISTRO_FEATURES','dhcpcd','dhcpcd','',d)} \
    ${DEBUG_TOOLS} \
    dosfstools \
    e2fsprogs \
    e2fsprogs-resize2fs \
    ${@bb.utils.contains('DISTRO_FEATURES','edac','edac-utils','',d)} \
    ethtool \
    file \
    ${@bb.utils.contains('MACHINE_FEATURES', 'gpu', d.getVar('GPU_USERLAND_LIBRARIES_INSTALL') or '', '', d)} \
    kmod \
    ldd \
    less \
    libstdc++ \
    ${@bb.utils.contains('DISTRO_FEATURES','wifi','linux-firmware-pcie8997','',d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'rauc', 'rauc', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'rauc', 'libdevmapper', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES','nxp-wifi','kernel-module-nxp89xx','',d)} \
    ${@bb.utils.contains('MACHINE_FEATURES','nxp-wifi','firmware-nxp-wifi-nxp8987-sdio','',d)} \
    lsof \
    mmc-utils \
    ncurses \
    net-tools \
    nfs-utils \
    ntp \
    ntp-utils \
    parted \
    ${@bb.utils.contains('DISTRO_FEATURES','pcie','pciutils','',d)} \
    rsync \
    sysfsutils \
    tcp-wrappers \
    tzdata \
    util-linux-fsck \
    util-linux-mkfs \
    util-linux-mount \
    util-linux-setterm \
    ${@bb.utils.contains('DISTRO_FEATURES','csi-camera','v4l-utils','',d)} \
    ${@bb.utils.contains('DISTRO_FEATURES','u-boot-fw-utils','u-boot-fw-utils u-boot-default-env','',d)} \
    ${@bb.utils.contains('DISTRO_FEATURES','wifi','wpa-supplicant','',d)} \
    ${@bb.utils.contains('DISTRO_FEATURES','bcm4373','linux-firmware-bcm4373','',d)} \
    ${@bb.utils.contains('DISTRO_FEATURES','copro','packagegroup-copro','',d)} \
"

IMAGE_INSTALL:append:use-nxp-bsp = " \
    ${@ bb.utils.contains('DISTRO_FEATURES', 'wifi', bb.utils.contains('DISTRO_FEATURES', 'systemd', " firmwared", "", d), "", d)} \
"

TTT_IP_INIT_PKGS = "kernel-module-edge kernel-module-st-stm32-deip bridge-utils"
TTT_IP_INIT_PKGS:append = "${@ \
    bb.utils.contains('DISTRO_FEATURES', 'systemd', " ttt-ip-switch", "", d) \
}"

CORE_IMAGE_EXTRA_INSTALL:append:stm32mp2 = "${@ \
    bb.utils.contains('MACHINE_FEATURES', 'stm-tsn-swch', d.getVar('TTT_IP_INIT_PKGS'), "", d) \
}"
