LICENSE = "MIT"

inherit core-image features_check

IMAGE_FSTYPES:append = " wic.zst"

require ${@ "karo-wic.inc" if any(fstype.startswith('wic') for fstype in d.getVar('IMAGE_FSTYPES').split()) else ""}

# Vendor specific flash package generation, e.g. 'nxp' on the NXP BSP
# (set through the use-nxp-bsp override) or the corresponding value on
# the mainline BSP. Fail with a clear error if the BSP layer did not
# set a vendor: the require below could not resolve otherwise.
KARO_PACKAGING_VENDOR:use-nxp-bsp = "nxp"

def karo_packaging_vendor_inc(d):
    vendor = (d.getVar('KARO_PACKAGING_VENDOR') or '').strip()
    if not vendor:
        bb.fatal("KARO_PACKAGING_VENDOR is not set on machine '%s': the "
                 "BSP layer must provide the flash packaging include "
                 "(e.g. KARO_PACKAGING_VENDOR = \"nxp\")" % d.getVar('MACHINE'))
    return "packaging/karo-%s.inc" % vendor

require ${@karo_packaging_vendor_inc(d)}

REQUIRED_DISTRO_FEATURES ??= ""

# Default partition sizes (KiB)
# The i.MX95 root filesystem (kernel modules, NXP firmware, debug tools)
# needs headroom for package management, hence the 192 MiB rootfs default.
BOOTFS_PARTITION_SIZE ??= "65536"
ROOTFS_PARTITION_SIZE ??= "196608"
USERFS_PARTITION_SIZE ??= "131072"

# qemuwrapper-cross for running postinst scripts at rootfs time
DEPENDS:append = " qemuwrapper-cross"

# Kernel image and devicetree files are provided by the dedicated
# boot partition, so don't install them into the rootfs images.
BAD_RECOMMENDATIONS += "kernel-image* kernel-devicetree"

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
