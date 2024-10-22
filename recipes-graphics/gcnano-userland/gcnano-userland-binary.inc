require gcnano-userland.inc

LICENSE = "Proprietary"
LICENSE_FLAGS = ""

LIC_FILES_CHKSUM = "file://${GCNANO_TAR_FILENAME}/LICENSE;md5=0e85fa534ee3a1a7f5ccc65cd6e789f9"

DEPENDS += " libdrm wayland "

SRC_URI = "git://github.com/STMicroelectronics/gcnano-binaries;protocol=https;branch=gcnano-${GCNANO_VERSION}-binaries"
SRCREV = "bbaae49a0e4859ed53f898a250269c8a237261bc"

PV = "${GCNANO_VERSION}+${GCNANO_DATE}"

GCNANO_VERSION = "6.4.15"

GCNANO_DATE:stm32mp1common = "20240206"
GCNANO_DATE:stm32mp2common = "20240517"

GCNANO_PACKAGECONFIG = "egl gbm glesv1 glesv2 vg"

GCNANO_PACKAGECONFIG:append = " ${@bb.utils.contains('MACHINE_FEATURES', 'openvx', 'vx', '', d)}"
GCNANO_PACKAGECONFIG:append = " ${@bb.utils.contains('MACHINE_FEATURES', 'opencl', 'cl', '', d)}"
GCNANO_PACKAGECONFIG:append = " ${@bb.utils.contains('MACHINE_FEATURES', 'vulkan', 'vulkan', '', d)}"

S = "${WORKDIR}/git"
B = "${S}/${GCNANO_TAR_FILENAME}"

#------------------------------------------
# Overwrite of do_unpack to untar the tarball
python() {
    # Make sure that we're dealing with recipe that enables externalsrc class
    if bb.data.inherits_class('externalsrc', d):
        bb.build.addtask('gcnano_userland_binaries_unpack', 'do_populate_lic', None, d)
        bb.build.addtask('gcnano_userland_binaries_unpack', 'do_configure', None, d)
}

do_gcnano_userland_binaries_unpack[depends] += "xz-native:do_populate_sysroot"
do_gcnano_userland_binaries_unpack() {
    cd ${S}
    rm -rf ${GCNANO_TAR_FILENAME}
    if [ -f "${GCNANO_TAR_FILENAME}.bin" ]; then
        sh ${GCNANO_TAR_FILENAME}.bin --auto-accept
    else
        bbfatal "Missing '${GCNANO_TAR_FILENAME}.bin' file in ${S} folder."
    fi
}

do_unpack[depends] += "xz-native:do_populate_sysroot"
python do_unpack() {
    eula = d.getVar('ACCEPT_EULA')
    eula_file = d.getVar('EULA_FILE_ST')
    machine = d.getVar('MACHINE')
    pkg = d.getVar('PN')
    if eula == None:
        bb.fatal("To use '%s' you need to accept the STMicroelectronics EULA at '%s'. "
                 "Please read it and in case you accept it, write: "
                 "ACCEPT_EULA = \"1\" in your local.conf." % (pkg, eula_file, machine))
    elif eula == '0':
        bb.fatal("To use '%s' you need to accept the STMicroelectronics EULA." % pkg)
    else:
        bb.note("STMicroelectronics EULA has been accepted for '%s'" % pkg)

    try:
        externalsrc = d.getVar('EXTERNALSRC')
        if not externalsrc:
            bb.build.exec_func('base_do_unpack', d)
            bb.build.exec_func('do_gcnano_userland_binaries_unpack', d)
    except:
        raise
}
do_unpack[vardepsexclude] += "EULA_FILE_ST"