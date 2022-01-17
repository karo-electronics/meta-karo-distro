LICENSE = "MIT"

inherit core-image features_check rootfs-postprocess relative_symlinks

ROOTFS_POSTPROCESS_COMMAND_remove = "\
        rootfs_update_timestamp; \
        ${bb.utils.contains('DISTRO_FEATURES','read-only-rootfs','empty_var_volatile;','',d)} \
"

REQUIRED_DISTRO_FEATURES ??= ""
IMAGE_FEATURES_append = " tools-debug"

IMAGE_INSTALL_append = " \
        alsa-utils-aplay \
        alsa-utils-amixer \
        alsa-utils-alsamixer \
        bash \
        canutils \
        coreutils \
        ${@bb.utils.contains('DISTRO_FEATURES','dhcpcd','dhcpcd','',d)} \
        dosfstools \
        e2fsprogs \
        e2fsprogs-resize2fs \
        file \
        gdbserver \
        i2c-tools \
        kmod \
        ldd \
        less \
        libstdc++ \
        libgpiod \
        libgpiod-tools \
        ${@bb.utils.contains('DISTRO_FEATURES','wifi','linux-firmware-pcie8997','',d)} \
        lsof \
        mmc-utils \
        ${@bb.utils.contains('MACHINE_FEATURES',"nand"," mtd-utils","",d)} \
        ncurses \
        nfs-utils \
        ntp \
        ntpdate \
        parted \
        ${@bb.utils.contains('DISTRO_FEATURES','pcie','pciutils','',d)} \
        rsync \
        strace \
        tcp-wrappers \
        tzdata \
        util-linux-fsck \
        util-linux-mkfs \
        util-linux-setterm \
        ${@bb.utils.contains('DISTRO_FEATURES','imx219','v4l-utils','',d)} \
        ${@bb.utils.contains('DISTRO_FEATURES','wifi','wpa-supplicant','',d)} \
"

DEPENDS_append = " cross-localedef-native qemuwrapper-cross"

CORE_IMAGE_EXTRA_INSTALL_append = " kernel-modules"
DEPENDS_append_mx6 = " u-boot-karo"
DEPENDS_append_mx8 = " imx-boot uuu-script-template"

PREFERRED_PROVIDER_fdisk = "util-linux"
PREFERRED_PROVIDER_virtual/bootloader = "u-boot-karo"
PREFERRED_PROVIDER_virtual/kernel = "linux-karo"

IMAGE_ROOTFS_ALIGNMENT ?= "${@bb.utils.contains('MACHINE_FEATURES',"nand",128*1024,1024,d)}"
IMAGE_ROOTFS_EXTRA_SPACE = "${@bb.utils.contains('MACHINE_FEATURES',"nand",2*128,0,d)}"
IMAGE_OVERHEAD_FACTOR = "1"
IMAGE_ROOTFS_SIZE ?= "${@${ROOTFS_PARTITION_SIZE}-(${IMAGE_ROOTFS_EXTRA_SPACE})}"

IMAGE_FSTYPES_remove = "tar.gz"
IMAGE_FSTYPES_append = " tar.bz2"
IMAGE_FSTYPES_append = "${@bb.utils.contains('MACHINE_FEATURES',"emmc"," ext4","",d)}"
IMAGE_FSTYPES_append = "${@bb.utils.contains('MACHINE_FEATURES',"nand"," ubi","",d)}"

MKUBIFS_LEB_SIZE = "126976"
MKUBIFS_LEB_CNT = "${@int(((${IMAGE_ROOTFS_SIZE} + ${IMAGE_ROOTFS_EXTRA_SPACE}) * 1024 + ${MKUBIFS_LEB_SIZE} - 1) / ${MKUBIFS_LEB_SIZE})}"
MKUBIFS_ARGS ?= "-F -x zlib -m 2048 -e ${MKUBIFS_LEB_SIZE} -c ${MKUBIFS_LEB_CNT}"

UBINIZE_ARGS ?= "-m 2048 -s 2048 -p 128KiB"

export MKUBIFS_ARGS_rootfs_qsmp-1510 = "-F -x zlib -m 2048 -e ${@d.getVar('MKUBIFS_LEB_SIZE')} -c ${@d.getVar('MKUBIFS_LEB_CNT')}"

IMAGE_FSTYPES_remove_qsmp-1510 = "ubi"
IMAGE_FSTYPES_append_qsmp-1510 = " multivol_ubi"

# UBI partition size in KiB
UBI_PARTITION_SIZE_qsmp-1510 = "128512"

MULTIUBI_VOLUMES_qsmp-1510 ?= " \
                               rootfs \
                               env \
                               dtb \
                               kernel \
                               userfs \
"

export UBIVOL_SIZE_rootfs = "32MiB"

export UBIVOL_SIZE_env = "128KiB"

export UBIVOL_TYPE_dtb = "static"
export UBIVOL_SIZE_dtb = "128KiB"
export UBIVOL_IMAGE_dtb = "stm32mp151a-qsmp-1510-qsbase1.dtb"

export UBIVOL_TYPE_kernel = "static"
export UBIVOL_SIZE_kernel = "16MiB"
export UBIVOL_IMAGE_kernel = "uImage"

export UBIVOL_SIZE_userfs = "${@128*1024 - 32*1024 - 2*128 - 16*1024}"
export UBIVOL_FLAGS_userfs = "autoresize"

#
# i.MX8 specific processing
#
# create bootfs image file with every karo-image-* build
do_image_complete[depends] += "karo-image-bootfs:do_image_complete"
do_image_complete[depends] += "uuu-script-template:do_deploy"

# create uuu.auto file for image if appropriate
python () {
    if "mx8" in d.getVar('MACHINEOVERRIDES').split(":"):
        bb.build.addtask('do_create_uuu_auto_script', 'do_build', 'do_image_complete', d)
}

python do_create_uuu_auto_script() {
    def do_copyfile (src, destdir):
        import shutil

        bb.note("Copying %s to %s" % (src, destdir))
        shutil.copyfile(src, os.path.join(destdir, os.path.basename(src)))

    bb.note("DEPLOY_DIR_IMAGE='%s'" % d.expand("${DEPLOY_DIR_IMAGE}"))
    bb.note("BPN='%s'" % d.expand("${BPN}"))

    from string import Template

    machine = d.getVar('MACHINE')
    imxboot_target = d.getVar('IMXBOOT_TARGETS')
    image = d.getVar('BPN')

    uuu_template_path = d.expand("${DEPLOY_DIR_IMAGE}/uuu.auto.template")
    img_dirname = d.expand("${DEPLOY_DIR_IMAGE}/${BPN}")
    uuu_auto_path = os.path.join(img_dirname, "uuu.auto")

    bb.utils.mkdirhier(img_dirname)

    f = open(uuu_template_path, 'r')
    uuu_template_string = f.read()

    try:
        with open(uuu_auto_path, 'w') as uuu_script:
            uuu_template = Template(uuu_template_string)
            uuu_auto = uuu_template.substitute(image=image, machine=machine, imxboot_target=imxboot_target)
            uuu_script.write(uuu_auto)
    except OSError:
        bb.fatal('Unable to open %s' % (uuu_auto_path))

    do_copyfile("%s/imx-boot-%s.bin-mfg-%s" % \
        (d.getVar('DEPLOY_DIR_IMAGE'), machine, imxboot_target), img_dirname)
    do_copyfile("%s/imx-boot-%s.bin-default-%s" % \
        (d.getVar('DEPLOY_DIR_IMAGE'), machine, imxboot_target), img_dirname)
    do_copyfile("%s/karo-image-bootfs-%s.ext4" % \
        (d.getVar('DEPLOY_DIR_IMAGE'), machine), img_dirname)
    do_copyfile("%s/%s-%s.ext4" % \
        (d.getVar('DEPLOY_DIR_IMAGE'), image, machine), img_dirname)
}

addtask do_clean_uuu_dir before do_clean

python do_clean_uuu_dir() {
    import glob

    for f in glob.glob(d.expand("${DEPLOY_DIR_IMAGE}/${BPN}/*")):
        bb.note("Removing %s" % f)
        os.remove(f)

    os.rmdir(d.expand("${DEPLOY_DIR_IMAGE}/${BPN}"))
}
