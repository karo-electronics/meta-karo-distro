POKY_DISTRO ??= "poky"
require conf/distro/${POKY_DISTRO}.conf
require ${@ "gpu_vivante.inc" if 'stm32mpcommon' in d.getVar('MACHINEOVERRIDES').split(":") else ""}
require ${@ "karo-nxp.inc" if 'use-nxp-bsp' in d.getVar('MACHINEOVERRIDES').split(":") else ""}
require ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', "systemd.inc", "", d)}

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
        nfs \
        seccomp \
        telnet-login \
	u-boot-fw-utils \
        usbgadget \
        usbhost \
        vfat \
        xattr \
"

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

IMAGE_CLASSES += "st-partitions-image"
IMAGE_CLASSES:remove = "image_populate_mfgtool"
IMAGE_NAME_SUFFIX = ""

ENABLE_PARTITIONS_IMAGE ?= "${@ bb.utils.contains('MACHINE_FEATURES', 'emmc', "1", "0", d)}"
PARTITIONS_IMAGE ?= "${@ "karo-image-bootfs karo-image-userfs" if ${ENABLE_PARTITIONS_IMAGE} == 1 else ""}"

DISTRO_FEATURES_DEFAULT:append:stm32mp2 = " csi-camera imx219"
DISTRO_FEATURES_DEFAULT:append:qsmp-1570w = " wifi bluetooth bcm4373"
DISTRO_FEATURES_DEFAULT:append:qsmp-1530w = " wifi bluetooth bcm4373"

VIRTUAL-RUNTIME_base-utils-hwclock = "util-linux-hwclock"
VIRTUAL-RUNTIME_dev_manager ??= "udev"

VOLATILE_LOG_DIR ?= "no"

MACHINE_FIRMWARE:remove = "${@ bb.utils.contains('DISTRO_FEATURES', 'wifi', "", "linux-firmware-ath10k", d)}"

ROOTFS_PARTITION_SIZE ??= "${@bb.utils.contains('MACHINE_FEATURES',"nand", \
                      65536-${IMAGE_ROOTFS_EXTRA_SPACE},128*1024,d)}"
BOOTFS_PARTITION_SIZE ??= "65536"

# list of mount points for subimages, that will be emptied in rootfs image
PARTITIONS_MOUNTPOINT_IMAGE ?= "/boot"

IMAGE_FSTYPES = "ext4 tar.bz2"

NETWORK_INTERFACES ??= "${@bb.utils.contains('MACHINE_FEATURES', 'stm-tsn-swch', "eth0 br0 sw0ep", "eth0", d)}"
NETWORK_INTERFACES_AUTO ??= "${@bb.utils.contains('MACHINE_FEATURES', 'stm-tsn-swch', "eth0", \
                             d.getVar('NETWORK_INTERFACES'), d)}"
DEFAULT_ETHERNET_MAIN_TSN_BRIDGE_INTERFACE ??= "${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'end0', 'eth0', d)}"
