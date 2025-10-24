FILESEXTRAPATHS:prepend := "${THISDIR}/patches:"

SRC_URI:append = " \
    file://deip-clk-bugfixes.patch \
"
