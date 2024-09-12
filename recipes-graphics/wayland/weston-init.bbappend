FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

PACKAGECONFIG:append:stm32mp2 = "${@ bb.utils.contains('MACHINE_FEATURES', 'gpu', "", " use-pixman", d)}"
