PACKAGES =+ "util-linux-setterm"

ALTERNATIVE:util-linux-setterm += "setterm"
ALTERNATIVE_LINK_NAME[setterm] = "${bindir}/setterm"
FILES:util-linux-setterm = "${bindir}/setterm*"
