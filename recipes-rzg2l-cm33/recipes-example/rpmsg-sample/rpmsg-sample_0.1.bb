#
# RPMsg Sample for Linux
#

SUMMARY = "Sample rpmsg application"
SECTION = "examples"
LICENSE = "CLOSED"
DEPENDS = "libmetal open-amp"
#INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
#INHIBIT_PACKAGE_STRIP = "1"


LDFLAGS+="-L${RECIPE_SYSROOT}${libdir}"

LDFLAGS+="-Wl,-rpath=/usr/local/lib"

prefix="/usr/local"
exec_prefix="/usr/local"

INSANE_SKIP:${PN} += "useless-rpaths"

SRC_URI = " \
    file://platform_info.c \
    file://platform_info.h \
    file://OpenAMP_RPMsg_cfg.h \
    file://helper.c \
    file://rsc_table.h \
    file://main.c \
    file://rzg2_rproc.c \
    file://Makefile"

S = "${WORKDIR}"

do_install() {
    install -d ${D}${prefix}
    install -m 0755 rpmsg_sample_client ${D}${prefix}
}



FILES:${PN} = "${prefix}/*"
