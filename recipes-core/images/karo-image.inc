LICENSE = "MIT"
EXTRA_IMAGE_FEATURES:remove = "read-only-rootfs"

def do_copyfile (src, destdir):
    bb.note("Copying %s to %s" % (src, destdir))
    if os.system("cp %s %s" % (src, destdir)):
        bb.fatal("Failed to copy %s to %s" % (src, destdir))

inherit core-image features_check rootfs-postprocess
inherit ${@ "flashlayout-stm32mp" if 'stm32mp1' in d.getVar('MACHINEOVERRIDES').split(":") else ""}
require ${@ "st-machine-common-stm32mp.inc" if 'stm32mp1' in d.getVar('MACHINEOVERRIDES').split(":") else ""}
require ${@ "karo-nxp.inc" if 'use-nxp-bsp' in d.getVar('MACHINEOVERRIDES').split(':') else "karo-mainline.inc"}
require ${@ "karo-rzg2.inc" if 'rzg2' in d.getVar('MACHINEOVERRIDES').split(':') else ""}
require ${@ "karo-stm.inc" if ('stm32mp1' in d.getVar('MACHINEOVERRIDES').split(':') and 'qsmp-1510' not in d.getVar('MACHINEOVERRIDES').split(':')) else ""}

inherit extrausers
EXTRA_USERS_PARAMS ?= "usermod -p '' root"

REQUIRED_DISTRO_FEATURES ??= ""

ROOTFS_POSTPROCESS_COMMAND:remove = "\
        rootfs_update_timestamp; \
        ${@bb.utils.contains('DISTRO_FEATURES','read-only-rootfs','empty_var_volatile;','',d)} \
"

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
        ethtool \
        file \
        kmod \
        ldd \
        less \
        libstdc++ \
        ${@bb.utils.contains('DISTRO_FEATURES','wifi','linux-firmware-pcie8997','',d)} \
        ${@bb.utils.contains('MACHINE_FEATURES','nxp-wifi','kernel-module-nxp89xx','',d)} \
        ${@bb.utils.contains('MACHINE_FEATURES','nxp-wifi','firmware-nxp-wifi-nxp8987-sdio','',d)} \
        lsof \
        mmc-utils \
        ${@bb.utils.contains('MACHINE_FEATURES',"nand"," mtd-utils","",d)} \
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
"

DEPENDS:append = " cross-localedef-native qemuwrapper-cross"

IMAGE_INSTALL:append = " kernel-modules"
DEPENDS:append:use-nxp-bsp = " uuu-script-template"
DEPENDS:append:rzg2 = " rzg2-install-scripts"
DEPENDS:append:stm32mp1 = " stm32mp1-install-scripts"

PREFERRED_PROVIDER_fdisk = "util-linux"

IMAGE_ROOTFS_ALIGNMENT ?= "${@bb.utils.contains('MACHINE_FEATURES',"nand",128*1024,1024,d)}"
IMAGE_ROOTFS_EXTRA_SPACE = "${@bb.utils.contains('MACHINE_FEATURES',"nand",2*128,0,d)}"
IMAGE_OVERHEAD_FACTOR = "1"
IMAGE_ROOTFS_SIZE ?= "${@${ROOTFS_PARTITION_SIZE}-(${IMAGE_ROOTFS_EXTRA_SPACE})}"

IMAGE_FSTYPES:append = "${@bb.utils.contains('MACHINE_FEATURES',"emmc"," ext4","",d)}"
IMAGE_FSTYPES:append = "${@bb.utils.contains('MACHINE_FEATURES',"nand"," ubi","",d)}"

# mkfs.ext4 parameters
# set inode size to 256 to support timestamps beyond 2038-01-19
EXTRA_IMAGECMD:ext4 += "-I 256"

# mkubifs parameters
MKUBIFS_LEB_SIZE = "126976"

MKUBIFS_ARGS ?= "-F -x zlib -m 2048 -e ${MKUBIFS_LEB_SIZE}"

UBINIZE_ARGS ?= "-m 2048 -s 2048 -p 128KiB"

PARTITION_NAMES[bootfs] ?= "boot"

def get_part_info(d, partlist, index):
    if d.getVar('ROOTFS_PARTITION_INDEX') != None:
        rootfs_partition_index = d.getVar('ROOTFS_PARTITION_INDEX')
    else:
        rootfs_partition_index = 1

    lastpart = index == len(partlist)

    if index == rootfs_partition_index:
        partname = "rootfs"
        image = d.getVar('BPN')
    else:
        i = index if index < rootfs_partition_index else index - 1
        partname = partlist[i]
        image = "karo-image-%s" % partname

    if partname in d.getVarFlags('IMAGE_ROOTFS_SIZE'):
        size = d.getVarFlags('IMAGE_ROOTFS_SIZE')[partname]
    elif d.getVar(partname.upper() + "_PARTITION_SIZE"):
        size = d.getVar(partname.upper() + "_PARTITION_SIZE")
    elif index == rootfs_partition_index:
        size = d.getVar('IMAGE_ROOTFS_SIZE')
    elif not lastpart:
        bb.fatal("Cannot determine size for partition '%s'" % partname)

    if lastpart:
        size = "MAX"
    elif size.isnumeric():
        size = size + "KiB"

    if d.getVarFlags('PARTITION_NAMES') and partname in d.getVarFlags('PARTITION_NAMES'):
        partname = d.getVarFlag('PARTITION_NAMES', partname, True)

    bb.note("partition[%s]='%s'" % (partname, size))
    return (partname, image, size)
