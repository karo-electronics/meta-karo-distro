FILESEXTRAPATHS:prepend := "${THISDIR}/files:${THISDIR}/patches:"

SRC_URI:append = " file://system.conf"

SRC_URI:append = " \
        file://0001-stm32-boot-handler.patch \
"
