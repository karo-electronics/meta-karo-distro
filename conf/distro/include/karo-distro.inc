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

# Ka-Ro images use a real (non-volatile) /var layout: drop the upstream
# permission tables that turn /var/log and /var/tmp into volatile
# symlinks, and apply the Ka-Ro permission deltas on top of the base
# table.
FILESYSTEM_PERMS_TABLES:remove = "files/fs-perms-volatile-log.txt files/fs-perms-volatile-tmp.txt"
FILESYSTEM_PERMS_TABLES:append = " meta/files/fs-perms-karo.txt"

# By default Ka-Ro images are not reproducible: they carry the build
# date in /.timestamp and SOURCE_DATE_EPOCH is unset, so packages may
# embed build timestamps. Enable the 'reproducible-timestamps' distro
# feature (fragment karo-distro/reproducible-timestamps) to restore the
# OE-Core reproducibility tasks.
BB_SIGNATURE_HANDLER = "OEBasicHash"
unset SOURCE_DATE_EPOCH

DEFAULT_TIMEZONE ?= "Etc/UTC"

ROOT_HOME ?= "/root"

IMAGE_PREPROCESS_COMMAND:remove = "${@ bb.utils.contains('DISTRO_FEATURES', 'reproducible-timestamps', "", "reproducible_final_image_task", d)}"
ROOTFS_POST_PROCESS_COMMAND:remove = "${@ bb.utils.contains('DISTRO_FEATURES', 'reproducible-timestamps', "", "rootfs_reproducible", d)}"
KERNEL_DEBUG_TIMESTAMPS = "${@ bb.utils.contains('DISTRO_FEATURES', 'reproducible-timestamps', "0", "1", d)}"
