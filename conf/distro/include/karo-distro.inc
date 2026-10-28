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
# NXP BSP kernels name their MACs 'endX' instead of 'ethX' when systemd's
# predictable interface naming is active; the mainline BSP keeps 'ethX'.
KARO_NET_IFPREFIX = "${@ 'eth' if 'use-mainline-bsp' in d.getVar('MACHINEOVERRIDES').split(':') else bb.utils.contains('DISTRO_FEATURES', 'systemd', 'end', 'eth', d)}"

# for fw_printenv/fw_setenv
UBOOT_INITIAL_ENV ?= "u-boot-initial-env"

IMAGE_NAME_SUFFIX = ""

# KARO_ROOTFS_READ_ONLY (set by the karo-distro/read-only-rootfs
# fragment) is the single switch for the read-only root filesystem.
# Read-only roots are only supported with sysvinit, so the effective
# flag is inactive with systemd. Recipes (e.g. base-files) consume the
# effective flag since they cannot see image features.
KARO_ROOTFS_READ_ONLY_ACTIVE = "${@ '1' if d.getVar('KARO_ROOTFS_READ_ONLY') == '1' and not bb.utils.contains('DISTRO_FEATURES', 'systemd', True, False, d) else '0' }"
EXTRA_IMAGE_FEATURES += "${@ 'read-only-rootfs' if d.getVar('KARO_ROOTFS_READ_ONLY_ACTIVE') == '1' else '' }"

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
