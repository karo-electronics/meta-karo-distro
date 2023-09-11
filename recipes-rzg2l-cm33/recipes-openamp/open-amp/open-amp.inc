SUMMARY = "Libopen_amp : Libmetal implements an abstraction layer across user-space Linux, baremetal, and RTOS environments"

HOMEPAGE = "https://github.com/OpenAMP/open-amp/"

SECTION = "libs"

LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=42f9861b7b1d8d0bcae2b1fd4fc656bc"


REPO ?= "git://github.com/OpenAMP/open-amp.git;protocol=https"
BRANCH ?= "main"

# Sources, by default allow for the use of SRCREV pointing to orphaned tags/commits
BRANCHARG = "${@['nobranch=1', 'branch=${BRANCH}'][d.getVar('BRANCH', True) != '']}"
SRC_URI = "${REPO};${BRANCHARG}"

S = "${WORKDIR}/git"

DEPENDS = "libmetal"

PROVIDES = "openamp"

inherit pkgconfig cmake yocto-cmake-translation

prefix="/usr/local"
exec_prefix="/usr/local"

OPENAMP_MACHINE_versal = "zynqmp"
OPENAMP_MACHINE ?= "${@get_cmake_machine(d.getVar('TARGET_OS'), d.getVar('TUNE_ARCH'), d.getVar('SOC_FAMILY'), d)}"
EXTRA_OECMAKE = " \
	-DLIB_INSTALL_DIR=${libdir} \
	-DLIBEXEC_INSTALL_DIR=${libdir} \
	-DMACHINE=${OPENAMP_MACHINE} \
	-DWITH_PROXY=off \
	-DWITH_PROXY_APPS=off \
	"

# OpenAMP apps not ready for Zynq
EXTRA_OECMAKE:append:zynqmp = "-DWITH_APPS=ON"
EXTRA_OECMAKE:append:versal = "-DWITH_APPS=ON"

ALLOW_EMPTY_${PN}-demos = "1"
PACKAGES:append = " ${PN}-demos"

FILES:${PN} = " \
    ${libdir}/*.so* \
"

FILES:${PN}-demos = " \
    ${bindir}/*-shared \
"
do_install:append () {
	# Only install echo test client, matrix multiplication client,
	# and proxy app server for ZynqMP
	rm -rf ${D}/${bindir}/*-static
}
