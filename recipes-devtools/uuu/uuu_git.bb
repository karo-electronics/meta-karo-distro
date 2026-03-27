SUMMARY = "Universal Update Utility"
DESCRIPTION = "Image deploy tool for i.MX chips"
HOMEPAGE = "https://github.com/nxp-imx/mfgtools"

SRC_URI = "git://github.com/nxp-imx/mfgtools.git;protocol=https;branch=master"
SRCREV = "da3cd53f056a7868fdffaa631e4e426847aec309"
PV = "1.5.182"

LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://LICENSE;md5=38ec0c18112e9a92cffc4951661e85a5"

inherit cmake pkgconfig

S = "${WORKDIR}/git"

DEPENDS = "libusb zlib bzip2 openssl zstd libtinyxml2"

BBCLASSEXTEND = "native nativesdk"

