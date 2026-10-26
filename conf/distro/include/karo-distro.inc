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

ethifname() {
    iface="$1"
    if ${@ "false" if 'use-mainline-bsp' in d.getVar('MACHINEOVERRIDES').split(':') else "true"};then
        echo "$iface"
    elif ${@ bb.utils.contains('DISTRO_FEATURES', 'systemd', "true", "false", d)};then
        echo "$iface" | sed s/eth/end/
    else
        echo "$iface"
    fi
}

# for fw_printenv/fw_setenv
UBOOT_INITIAL_ENV ?= "u-boot-initial-env"

IMAGE_CLASSES:remove = "image_populate_mfgtool"
IMAGE_NAME_SUFFIX = ""

VIRTUAL-RUNTIME_base-utils-hwclock = "util-linux-hwclock"
VIRTUAL-RUNTIME_dev_manager ??= "udev"

PACKAGECONFIG:append:pn-gstreamer1.0-plugins-ugly = " x264"

TOOLCHAIN_HOST_TASK:append = " \
    nativesdk-binutils \
    nativesdk-gcc nativesdk-gcc-symlinks \
    nativesdk-gnutls-dev \
"

IMX_MALI_DRIVERS ??= "mali-imx mesa"

PACKAGECONFIG:append:pn-pulseaudio = " autospawn-for-root"

PACKAGECONFIG:append:pn-mesa-demos = " wayland"
PACKAGECONFIG:remove:pn-mesa-demos = "vulkan"

BB_SIGNATURE_HANDLER = "OEBasicHash"
unset SOURCE_DATE_EPOCH

DEFAULT_TIMEZONE ?= "Etc/UTC"

ROOT_HOME ?= "/root"

IMAGE_PREPROCESS_COMMAND:remove = "${@ bb.utils.contains('DISTRO_FEATURES', 'reproducible-timestamps', "", "reproducible_final_image_task", d)}"
ROOTFS_POST_PROCESS_COMMAND:remove = "${@ bb.utils.contains('DISTRO_FEATURES', 'reproducible-timestamps', "", "rootfs_reproducible", d)}"
KERNEL_DEBUG_TIMESTAMPS = "${@ bb.utils.contains('DISTRO_FEATURES', 'reproducible-timestamps', "0", "1", d)}"

# convert update alternatives to relative symlinks
INHERIT += "${@bb.utils.contains('DISTRO_FEATURES', 'alternatives-symlinks-relative', 'update-alternatives-relative', '', d)}"
