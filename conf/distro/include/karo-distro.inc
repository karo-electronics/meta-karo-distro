POKY_DISTRO ??= "poky"
require conf/distro/${POKY_DISTRO}.conf

DISTRO_VERSION = "${PREFERRED_VERSION_linux-karo}-${DISTRO_CODENAME}"

POKY_DEFAULT_DISTRO_FEATURES:remove = "\
        largefile \
        opengl \
        ptest \
        vulkan \
        wayland \
"

DISTRO_FEATURES_DEFAULT ?= "\
        acl \
        alsa \
        argp \
        dhcpcd \
        ext2 \
        ipv4 \
        largefile \
        ldconfig \
        pam \
        usbgadget \
        usbhost \
        xattr \
        nfs \
        telnet-login \
        vfat \
        seccomp \
"

IMAGE_CLASSES += "st-partitions-image"

ENABLE_PARTITIONS_IMAGE ?= "${@ bb.utils.contains('MACHINE_FEATURES', 'emmc', "1", "0", d)}"
PARTITIONS_IMAGE ?= "${@ "karo-image-bootfs karo-image-userfs" if ${ENABLE_PARTITIONS_IMAGE} == 1 else ""}"

DISTRO_FEATURES_DEFAULT:append:mx8-nxp-bsp  = " dsi83"
DISTRO_FEATURES_DEFAULT:append:mx8mp-nxp-bsp  = " flexcan"
DISTRO_FEATURES_DEFAULT:append:tx8m = " pcie"
DISTRO_FEATURES_DEFAULT:append:qs8m = " csi-camera imx219 raspi-display tc358867"
DISTRO_FEATURES_DEFAULT:append:qs93 = " csi-camera imx219"
DISTRO_FEATURES_DEFAULT:append:qsxm = " basler csi-camera imx219 pcie raspi-display tc358867"
DISTRO_FEATURES_DEFAULT:append:qsxp = " basler csi-camera pcie raspi-display tc358867"
DISTRO_FEATURES_DEFAULT:append:tx8p = " lvds pcie"

VIRTUAL-RUNTIME_base-utils-hwclock = "util-linux-hwclock"
VIRTUAL-RUNTIME_dev_manager ??= "udev"
VIRTUAL-RUNTIME_init_manager ??= "${@bb.utils.contains('DISTRO_FEATURES',"systemd","systemd","sysvinit",d)}"

PREFERRED_PROVIDER_opencl-headers = "imx-gpu-viv"
PREFERRED_RPROVIDER_mfgtool-base ?= "linux-imx-mfgtool"
PREFERRED_RPROVIDER_mfgtool-image ?= "${PREFERRED_RPROVIDER_mfgtool-base}"
PREFERRED_RPROVIDER_mfgtool-image-image ?= "${PREFERRED_RPROVIDER_mfgtool-base}"

VOLATILE_LOG_DIR ?= "no"

MACHINE_FIRMWARE:remove = "${@ bb.utils.contains('DISTRO_FEATURES', 'wifi', "", "linux-firmware-ath10k", d)}"

ROOTFS_PARTITION_SIZE ??= "${@bb.utils.contains('MACHINE_FEATURES',"nand", \
                      65536-${IMAGE_ROOTFS_EXTRA_SPACE},128*1024,d)}"
BOOTFS_PARTITION_SIZE ??= "65536"

# list of mount points for subimages, that will be emptied in rootfs image
PARTITIONS_MOUNTPOINT_IMAGE ?= "/boot"

IMAGE_FSTYPES = "ext4 tar.bz2"

NETWORK_INTERFACES ?= "eth0"
NETWORK_INTERFACES:mx9-nxp-bsp ?= "eth0 eth1"
NETWORK_INTERFACES:mx8mp-nxp-bsp ?= "eth0 eth1"

# mask out sysvinit recipes from meta-imx layer
BBMASK:append:use-nxp-bsp = " .*/meta-imx/.*/sysvinit/.*"
# mask out silly weston-init.bbappend from meta-imx layer
BBMASK:append:use-nxp-bsp = " .*/meta-imx/.*/weston-init.bbappend"
