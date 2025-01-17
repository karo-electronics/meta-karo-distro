FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI:remove = "\
	file://start_getty \
"

do_install () {
    install -d ${D}${sysconfdir}
    install -m 0644 ${WORKDIR}/inittab ${D}${sysconfdir}/inittab

    i=0
    cons="${SERIAL_CONSOLES}"
    for c in ${cons};do
	if [ $i = 0 ];then
	    grep -q '# Serial consoles on the standard serial ports' ${D}${sysconfdir}/inittab && break
	    cat <<EOF >> ${D}${sysconfdir}/inittab
#
# Serial consoles on the standard serial ports
EOF
	fi
	speed=$(echo ${c} | cut -d\; -f 1)
	device=$(echo ${c} | cut -d\; -f 2)
	label=$(echo ${device} | sed s/tty// | tail -n 5)
	echo "${label}:123:respawn:${sbindir}/ttyrun ${device} ${base_sbindir}/getty -L ${speed} ${device} linux" >> ${D}${sysconfdir}/inittab
	i=`expr $i + 1`
    done

    if [ "${USE_VT}" = "1" ]; then
        cat <<EOF >> ${D}${sysconfdir}/inittab
#
# ${base_sbindir}/getty invocations for the runlevels.
#
# The "id" field MUST be the same as the last
# characters of the device (after "tty").
#
# Format:
#  <id>:<runlevels>:<action>:<process>
EOF
        for n in ${SYSVINIT_ENABLED_GETTYS};do
            echo "$n:345:respawn:${base_sbindir}/getty 38400 tty$n" >> ${D}${sysconfdir}/inittab
        done
        echo "" >> ${D}${sysconfdir}/inittab
    fi

    if ${@ bb.utils.contains('DISTRO_FEATURES','telnet-login','true','false',d)};then
	if ! grep -q '# Telnet daemon' ${D}${sysconfdir}/inittab;then
	    cat << EOF >> ${D}${sysconfdir}/inittab

# Telnet daemon
T0:2345:respawn:/usr/sbin/telnetd -F
EOF
	fi
    fi
}
