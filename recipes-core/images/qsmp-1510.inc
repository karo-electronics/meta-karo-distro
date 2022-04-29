ROOTFS_PARTITION_SIZE ??= "32*1024"

IMAGE_FSTYPES_remove = "ubi"
IMAGE_FSTYPES_append = " multivol_ubi"

# UBI partition size in KiB

MULTIUBI_VOLUMES ?= " \
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
export UBIVOL_SIZE_userfs = "128KiB"
export UBIVOL_FLAGS_userfs = "autoresize"
