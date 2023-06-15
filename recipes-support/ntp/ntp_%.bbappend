inherit relative_symlinks useradd

PACKAGECONFIG:remove = "${@ bb.utils.contains('IMAGE_INSTALL','openssl',"","openssl",d)}"

remove_ntp_conf = "${@ bb.utils.contains('DISTRO_FEATURES', 'dhcpcd', True, False, d)}"

FILES:${PN}:remove := "${@ "${sysconfdir}/ntp.conf" if ${remove_ntp_conf} else ""}"

DEPENDS += "busybox"

USERADD_PACKAGES += "${PN}"
USERADD_PARAM:${PN} = "--system -U -d /nonexistent --no-create-home -s /bin/false ntp"
GROUPADD_PARAM:${PN} = "-f --system crontab"

CRONTABS_DIR = "${localstatedir}/spool/cron"
FILES:${PN} += "${CRONTABS_DIR}/root"

do_install:append() {
    if [ "${remove_ntp_conf}" = "True" ];then
        rm -f ${D}${sysconfdir}/ntp.conf
    fi
}

pkg_postinst:ntpdate() {
    if ! grep -q -s ntpdate $D${CRONTABS_DIR}/root; then
        echo "adding crontab"
        test -d $D${CRONTABS_DIR} || install -d -g crontab -m 1730 $D${CRONTABS_DIR}
        echo "30 * * * *    ${bindir}/ntpdate-sync silent" >> $D${CRONTABS_DIR}/root
    fi
}
