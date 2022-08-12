FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI:remove = "\
	       file://nobash.patch \
"
