POKY_DISTRO ??= "poky"
require conf/distro/${POKY_DISTRO}.conf

# The init manager is selected through the standard INIT_MANAGER
# variable (conf/distro/include/init-manager-${INIT_MANAGER}.inc):
# set INIT_MANAGER = "systemd" in the configuration to build with
# systemd instead of the default sysvinit.

DISTRO_VERSION = "6.18-${DISTRO_CODENAME}"

POKY_DEFAULT_DISTRO_FEATURES:remove = "\
    largefile \
    opengl \
    ptest \
    vulkan \
    wayland \
"

DISTRO_FEATURES_DEFAULTS ?= "\
    acl \
    alsa \
    argp \
    dhcpcd \
    ext2 \
    ipv4 \
    largefile \
    ldconfig \
    pam \
    nfs \
    seccomp \
    u-boot-fw-utils \
    usbgadget \
    usbhost \
    vfat \
    xattr \
"
DISTRO_FEATURES_DEFAULTS:append:mx9-nxp-bsp = " edac"

# Prefix of the network interface names as seen by the target kernel.
# The kernel names the MACs 'ethX' by probe order. Under systemd, udev's
# predictable naming derives 'endX' from the devicetree (the 'ethernetN'
# aliases on the NXP BSP network endpoints); eudev has no devicetree
# support, so sysvinit keeps 'ethX'. Mainline BSP machines keep 'ethX'
# either way.
KARO_NET_IFPREFIX = "eth"
KARO_NET_IFPREFIX:use-nxp-bsp = "${@ bb.utils.contains('DISTRO_FEATURES', 'systemd', 'end', 'eth', d)}"

# for fw_printenv/fw_setenv
UBOOT_INITIAL_ENV ?= "u-boot-initial-env"

IMAGE_NAME_SUFFIX = ""

# The read-only-rootfs distro feature (set by the
# karo-distro/read-only-rootfs fragment) is the single switch for the
# read-only root filesystem. Recipes (e.g. base-files) consume it
# directly, since recipes cannot see image features, and the image
# feature of the same name is derived from it here. Both init managers
# are supported: sysvinit through populate-volatile.sh and the
# read-only-rootfs hook, systemd through its tmpfiles setup and the
# volatile-binds copy mounts (a hard systemd RDEPENDS).
EXTRA_IMAGE_FEATURES += "${@bb.utils.contains('DISTRO_FEATURES', 'read-only-rootfs', ' read-only-rootfs', '', d)}"

VIRTUAL-RUNTIME_base-utils-hwclock = "util-linux-hwclock"

# busybox and util-linux both provide fdisk
PREFERRED_PROVIDER_fdisk = "util-linux"

PACKAGECONFIG:append:pn-gstreamer1.0-plugins-ugly = " x264"

# The Ka-Ro images do not rely on the openssl features of these recipes.
# Recipes must not inspect IMAGE_INSTALL (image content) to pick their
# PACKAGECONFIG: decide once, at distro level.
PACKAGECONFIG:remove:pn-ntp = "openssl"
PACKAGECONFIG:remove:pn-tcpdump = "openssl"

TOOLCHAIN_HOST_TASK:append = " \
    nativesdk-binutils \
    nativesdk-gcc nativesdk-gcc-symlinks \
    nativesdk-gnutls-dev \
"

IMX_MALI_DRIVERS ??= "mali-imx mesa"

PACKAGECONFIG:append:pn-pulseaudio = " autospawn-for-root"

PACKAGECONFIG:append:pn-mesa-demos = " wayland"
PACKAGECONFIG:remove:pn-mesa-demos = "vulkan"

DEFAULT_TIMEZONE ?= "Etc/UTC"

ROOT_HOME ?= "/root"
