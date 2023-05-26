DESCRIPTION = "Package group for coprocessor firmwares"

inherit packagegroup

PACKAGE_ARCH = "${MACHINE_ARCH}"

COPRO_PKGS = ""

COPRO_PKGS:stm32mp1 = " \
    m4-stm32mp1 \
"

COPRO_PKGS:rzg2l = " \
	rpmsg-sample \
"

RDEPENDS:${PN} += " \
    ${COPRO_PKGS} \
"
