LICENSE = "MIT"

FILESEXTRAPATHS:prepend := "${THISDIR}/uuu-script-templates:"

# Set to "1" to enable STM32CubeProgrammer flashlayout generation for
# STM32MP-based machines. This includes st-machine-common-stm32mp.inc,
# which defines the boot/rootfs/userfs partition images and install
# scripts used by STM32CubeProgrammer to flash the board.
KARO_STM32_CUBE_PROGRAMMER ?= "0"

def do_copyfile (src, destdir):
    bb.note("Copying %s to %s" % (src, destdir))
    if os.system("cp %s %s" % (src, destdir)):
        bb.fatal("Failed to copy %s to %s" % (src, destdir))

inherit core-image features_check rootfs-postprocess

# UBI image for NAND modules, otherwise use WIC image format
require ${@ "karo-ubi.inc" if 'nand' in d.getVar('MACHINE_FEATURES').split() else "karo-wic.inc"}

# Vendor specific flash script generation for NXP, RZG2 and STM32MP-based machines
require ${@ "karo-nxp.inc" if 'use-nxp-bsp' in d.getVar('MACHINEOVERRIDES').split(':') else ""}
require ${@ "karo-rzg2.inc" if 'rzg2' in d.getVar('MACHINEOVERRIDES').split(':') else ""}
require ${@ "karo-stm.inc" if 'stm32mpcommon' in d.getVar('MACHINEOVERRIDES').split(':') else ""}

# STM32CubeProgrammer flashlayout generation
require ${@ "st-machine-common-stm32mp.inc" if 'stm32mpcommon' in d.getVar('MACHINEOVERRIDES').split(':') and d.getVar('KARO_STM32_CUBE_PROGRAMMER') == '1' else ""}

inherit extrausers
EXTRA_USERS_PARAMS ?= "usermod -p '' root"

REQUIRED_DISTRO_FEATURES ??= ""

ROOTFS_POSTPROCESS_COMMAND:remove = "\
    rootfs_update_timestamp \
    ${@bb.utils.contains('IMAGE_FEATURES','read-only-rootfs','empty_var_volatile','',d)} \
"

python () {
    # Needed for template files
    d.delVarFlag('do_unpack', 'noexec')
}

# Default partition sizes
BOOTFS_PARTITION_SIZE ??= "65536"
ROOTFS_PARTITION_SIZE ??= "131072"
USERFS_PARTITION_SIZE ??= "131072"

UUU_TEMPLATE_NAME ??= "uuu.auto.template"

SRC_URI:append = " file://${UUU_TEMPLATE_NAME}"

DEPENDS:append = " cross-localedef-native qemuwrapper-cross"

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

IMAGE_INSTALL:append = " kernel-modules"

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

# U-Boot overwrites files in /boot: no symlinks there
IMAGE_PREPROCESS_COMMAND:append = " karo_bootfs_replace_symlinks"

karo_bootfs_replace_symlinks() {
    bootfs="${IMAGE_ROOTFS}/boot"
    [ -d "$bootfs" ] || return 0
    for f in "$bootfs"/*; do
        if [ -h "$f" ]; then
            target="$(readlink "$f")"
            if [ -f "$bootfs/$target" ]; then
                bbnote "Replacing symlink '$f' with actual file '$target'"
                mv -v "$bootfs/$target" "$f"
            else
                rm -v "$f"
            fi
        fi
    done
}

python do_export_sbom() {
    machine = d.getVar('MACHINE')
    rootfs_image = d.getVar('BPN')
    img_dirname = os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), rootfs_image)
    
    bb.utils.mkdirhier(img_dirname)
    do_copyfile("%s/%s-%s.spdx.json" % \
        (d.getVar('SPDXIMAGEDEPLOYDIR'), rootfs_image, machine), img_dirname)
}
do_create_image_sbom_spdx[postfuncs] += "do_export_sbom"
do_create_image_sbom_spdx_setscene[postfuncs] += "do_export_sbom"
