LICENSE = "MIT"

inherit core-image distro_features_check rootfs-postprocess relative_symlinks

ROOTFS_POSTPROCESS_COMMAND_remove = "\
				     rootfs_update_timestamp; \
				     ${bb.utils.contains('DISTRO_FEATURES','read-only-rootfs','empty_var_volatile;','',d)} \
"

IMAGE_INSTALL_append = " \
              ${CORE_IMAGE_EXTRA_INSTALL} \
              alsa-utils-aplay \
              alsa-utils-amixer \
              alsa-utils-alsamixer \
              bash \
              canutils \
              coreutils \
              dhcpcd \
              dosfstools \
              e2fsprogs \
              file \
              gdbserver \
              kmod \
              i2c-tools \
              ldd \
	      less \
              libstdc++ \
              lsof \
              mmc-utils \
	      ncurses \
              ntp \
              ntpdate \
              parted \
              pciutils \
              rsync \
              strace \
	      tcp-wrappers \
              tzdata \
              util-linux-fsck \
              util-linux-mkfs \
              util-linux-setterm \
"

CORE_IMAGE_EXTRA_INSTALL_append = " kernel-modules"
DEPENDS_append = " u-boot-karo"

PREFERRED_PROVIDER_fdisk = "util-linux"
PREFERRED_PROVIDER_virtual/bootloader = "u-boot-karo"
PREFERRED_PROVIDER_virtual/kernel = "linux-karo"

IMAGE_ROOTFS_SIZE ?= "${@bb.utils.contains('MACHINE_FEATURES',"nand",64*1024,128*1024,d)}"
IMAGE_ROOTFS_ALIGNMENT ?= "${@bb.utils.contains('MACHINE_FEATURES',"nand",128*1024,1024,d)}"
IMAGE_ROOTFS_EXTRA_SPACE ?= "${@bb.utils.contains('MACHINE_FEATURES',"nand",2*128*1024,0,d)}"

IMAGE_FSTYPES_remove = "tar.gz"
IMAGE_FSTYPES_append = " tar.bz2"
IMAGE_FSTYPES_append = "${@bb.utils.contains('MACHINE_FEATURES',"nand"," ubi","",d)}"
IMAGE_FSTYPES_append = "${@bb.utils.contains('MACHINE_FEATURES',"emmc"," ext4","",d)}"

MKUBIFS_ARGS ?= "-F -x zlib -m 2048 -e 126976 -c 256"
UBINIZE_ARGS ?= "-m 2048 -s 2048 -p 128KiB"
UBI_VOLNAME = "rootfs"
