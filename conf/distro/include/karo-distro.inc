POKY_DISTRO ??= "poky"
require conf/distro/${POKY_DISTRO}.conf
require ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', "systemd.inc", "", d)}

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
    telnet-login \
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

IMAGE_CLASSES:remove = "image_populate_mfgtool"
IMAGE_NAME_SUFFIX = ""

VIRTUAL-RUNTIME_base-utils-hwclock = "util-linux-hwclock"
VIRTUAL-RUNTIME_dev_manager ??= "udev"

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

# With a read-only root filesystem the volatile tables must stay enabled:
# logs and tmp end up on the /var/volatile tmpfs, since only /var/lib is
# made writable by the upstream read-only-rootfs hook. On a writable root
# the tables are dropped, which gives persistent /var/log and /var/tmp
# (the tables double as the switch consumed by base-files and initscripts).
FILESYSTEM_PERMS_TABLES:remove = "${@ '' if d.getVar('KARO_ROOTFS_READ_ONLY') == '1' else 'files/fs-perms-volatile-log.txt files/fs-perms-volatile-tmp.txt'}"
FILESYSTEM_PERMS_TABLES:append = " meta/files/fs-perms-karo.txt"

# Reproducible package and image timestamps follow the OE-Core defaults
# (SOURCE_DATE_EPOCH stays set, which is also required by the OEEquivHash
# signature handler).

DEFAULT_TIMEZONE ?= "Etc/UTC"

ROOT_HOME ?= "/root"
