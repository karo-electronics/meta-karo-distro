SUMMARY = "Libmetal : Libmetal implements an abstraction layer across user-space Linux, baremetal, and RTOS environments"

HOMEPAGE = "https://github.com/OpenAMP/libmetal/"

SECTION = "libs"

LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=b708248317264631cf013ef77184c163"

REPO ?= "git://github.com/OpenAMP/libmetal.git;protocol=https"
BRANCH ?= "main"

prefix="/usr/local"
exec_prefix="/usr/local"

# Sources, by default allow for the use of SRCREV pointing to orphaned tags/commits
BRANCHARG = "${@['nobranch=1', 'branch=${BRANCH}'][d.getVar('BRANCH', True) != '']}"
SRC_URI = "${REPO};${BRANCHARG}"

S = "${WORKDIR}/git"

# if user want to enable hugetlbfs support, please
# add "DEPENDS_append_pn-libmetal = " libhugetlbfs "" to your local.conf
DEPENDS = "sysfsutils"

PROVIDES = "libmetal"

inherit pkgconfig cmake yocto-cmake-translation

LIBMETAL_MACHINE_versal = "zynqmp"
LIBMETAL_MACHINE ?= "${@get_cmake_machine(d.getVar('TARGET_OS'), d.getVar('TUNE_ARCH'), d.getVar('SOC_FAMILY'), d)}"


EXTRA_OECMAKE = " \
	-DLIB_INSTALL_DIR=${libdir} \
	-DLIBEXEC_INSTALL_DIR=${libexecdir} \
	-DMACHINE=${LIBMETAL_MACHINE} \
	-DWITH_EXAMPLES=ON \
	"
ALLOW_EMPTY:${PN}-demos = "1"

PACKAGES:append = " ${PN}-demos"



FILES:${PN} = " \
	${libdir}/*.so* \
	${bindir}/test-* \
"
FILES:${PN}-demos = " \
	${bindir}/libmetal_* \
"

do_install:append ()  {


	# Only install the shared lib binaries
	rm -rf ${D}/${bindir}/*-static

	for f in `find ${D}/${bindir}/ -type f -regex ".*-shared*"`; do
		newf="`echo ${f} | sed 's/-shared*$//g'`"
		mv "${f}" "${newf}"
	done
}
