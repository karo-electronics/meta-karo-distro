SUMMARY = "Volatile bind mount setup and configuration for read-only-rootfs"
DESCRIPTION = "${SUMMARY}"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://COPYING.MIT;md5=5750f3aa4ea2b00c2bf21b2b2a7b714d"

SRC_URI_karo_minimal = ""

inherit allarch features_check

VOLATILE_BINDS_karo_minimal = ""
VOLATILE_BINDS_karo_minimal[type] = "list"
VOLATILE_BINDS_karo_minimal[separator] = "\n"

SYSTEMD_SERVICE_${PN}_karo_minimal = ""

FILES_${PN}_karo_minimal = ""

do_compile_karo_minimal () {
}
do_compile_karo_minimal[dirs] = "${WORKDIR}"

do_install_karo_minimal () {
    install -d ${D}${sysconfdir}
    install -m 0755 ${D}${sysconfdir}.var
    install -m 0755 ${D}${sysconfdir}.run
}
do_install_karo_minimal[dirs] = "${WORKDIR}"
