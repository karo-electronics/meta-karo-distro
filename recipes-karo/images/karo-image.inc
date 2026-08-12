LICENSE = "MIT"

def do_copyfile (src, destdir):
    bb.note("Copying %s to %s" % (src, destdir))
    if os.system("cp %s %s" % (src, destdir)):
        bb.fatal("Failed to copy %s to %s" % (src, destdir))

inherit core-image features_check rootfs-postprocess

require ${@ "karo-nxp.inc" if 'use-nxp-bsp' in d.getVar('MACHINEOVERRIDES').split(':') else ""}
require ${@ "karo-rzg2.inc" if 'rzg2' in d.getVar('MACHINEOVERRIDES').split(':') else ""}
require ${@ "karo-stm.inc" if 'stm32mpcommon' in d.getVar('MACHINEOVERRIDES').split(':') else ""}

require ${@ "st-machine-common-stm32mp.inc" if 'stm32mpcommon' in d.getVar('MACHINEOVERRIDES').split(":") else ""}

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

# Partitioning using wic image format
IMAGE_FSTYPES = "wic.zst"

# Default partition sizes
BOOTFS_PARTITION_SIZE ??= "65536"
ROOTFS_PARTITION_SIZE ??= "131072"
USERFS_PARTITION_SIZE ??= "131072"

FILESEXTRAPATHS:prepend := "${THISDIR}/uuu-script-templates:"
SRC_URI:append = " file://uuu.auto.template"
SRC_URI:append:stm32mpcommon = " file://start-fastboot.template"
SRC_URI:append:rzg2 = " file://install-bootloader.template"

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

IMAGE_INSTALL:append = " kernel-modules kernel-devicetree kernel-image"

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

BINARIES_TARGET_BASENAME:stm32mpcommon = "${SOC_FAMILY}-${MACHINE}-${FLASHLAYOUT_BOOTSCHEME_LABELS}"
BINARIES_TARGET_BASENAME:stm32mpcommon:append = "${@ bb.utils.contains('SIGN_ENABLE', '1', "_Signed", "", d)}"

WKS_FILE_BASE = "karo-image"
WKS_FILE_BASE:stm32mpcommon = "karo-image-st"

WKS_FILE_BASE:append = "${@ bb.utils.contains('DISTRO_FEATURES', 'rauc', '-rauc', '', d)}"

WKS_FILE = "${WKS_FILE_BASE}.wks.in"

WIC_ROOTFS_PARTITIONS_EXPORT = "boot rootfs userfs"

python do_export_wic_partitions() {
    import subprocess

    def get_partition_meta(path):
        out = subprocess.run(
            ["blkid", "-o", "export", path], text=True, capture_output=True
        ).stdout.splitlines()
        data = dict(line.split("=", 1) for line in out)
        return data.get("LABEL", "img"), data.get("TYPE", "bin")

    build_wic = os.path.join(d.getVar("WORKDIR"), "build-wic")
    deploy_dir = d.getVar("IMGDEPLOYDIR")
    image_basename = d.getVar("IMAGE_BASENAME")
    machine = d.getVar("MACHINE")
    export_partitions = d.getVar("WIC_ROOTFS_PARTITIONS_EXPORT").split()

    for fname in os.listdir(build_wic):
        if ".direct.p" not in fname:
            continue
        path = os.path.join(build_wic, fname)
        fsname, fstype = get_partition_meta(path)
        if fsname not in export_partitions:
            continue
        dest_basename = f"{image_basename}-{machine}.{fsname}"
        bb.utils.copyfile(path, os.path.join(deploy_dir, f"{dest_basename}.{fstype}"))
        path = os.path.join(build_wic, f"{fsname}.tar.bz2")
        bb.utils.copyfile(path, os.path.join(deploy_dir, f"{dest_basename}.tar.bz2"))
}
addtask export_wic_partitions after do_image_wic before do_image_complete

python do_add_sbom() {
    machine = d.getVar('MACHINE')
    rootfs_image = d.getVar('BPN')
    img_dirname = os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), d.getVar('BPN'))
    
    bb.utils.mkdirhier(img_dirname)
    do_copyfile("%s/%s-%s.spdx.json" % \
        (d.getVar('SPDXIMAGEDEPLOYDIR'), rootfs_image, machine), img_dirname)
}
do_create_image_sbom_spdx[postfuncs] += "do_add_sbom"
do_create_image_sbom_spdx_setscene[postfuncs] += "do_add_sbom"
