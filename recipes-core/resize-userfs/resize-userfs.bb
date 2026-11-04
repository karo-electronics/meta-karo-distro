SUMMARY = "Grow the userfs partition into the remaining storage space on first boot"
DESCRIPTION = "On the first boot after flashing, grow the userfs partition (/usr/local) \
and its ext4 filesystem into the free space left on the storage device."
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
    file://resize-userfs.sh \
    ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'file://resize-userfs.service', 'file://resize-userfs.init', d)} \
"

S = "${UNPACKDIR}"

# Install exactly one init implementation, matching the init manager of
# the build: the inherited class enables the unit or the init script at
# image assembly time.
inherit ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'systemd', 'update-rc.d', d)}

do_install() {
    install -d ${D}${sbindir}
    install -m 0755 ${S}/resize-userfs.sh ${D}${sbindir}/resize-userfs

    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
        install -d ${D}${systemd_system_unitdir}
        install -m 0644 ${S}/resize-userfs.service ${D}${systemd_system_unitdir}
    else
        install -d ${D}${sysconfdir}/init.d
        install -m 0755 ${S}/resize-userfs.init ${D}${sysconfdir}/init.d/resize-userfs
    fi
}

# Runs after the local filesystems are mounted (local-fs.target on
# systemd, mountall.sh in runlevel S on sysvinit), so resize2fs can
# grow the ext4 filesystem online.
SYSTEMD_SERVICE:${PN} = "resize-userfs.service"

INITSCRIPT_NAME = "resize-userfs"
INITSCRIPT_PARAMS = "start 06 S ."

FILES:${PN} += "${systemd_system_unitdir}/resize-userfs.service"

RDEPENDS:${PN} = " \
    e2fsprogs-resize2fs \
    util-linux-partx \
    util-linux-sfdisk \
"
