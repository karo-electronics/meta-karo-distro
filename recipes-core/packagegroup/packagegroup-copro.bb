DESCRIPTION = "Package group for coprocessor firmwares"

inherit packagegroup

COPRO_PKGS = ""

COPRO_PKGS:stm32mp1 = " \
    m4-stm32mp1 \
"

RDEPENDS:${PN} += " \
    ${COPRO_PKGS} \
"
