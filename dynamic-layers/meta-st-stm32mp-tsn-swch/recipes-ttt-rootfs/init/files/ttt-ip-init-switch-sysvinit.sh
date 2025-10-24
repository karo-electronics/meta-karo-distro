#!/bin/sh
set -x

# SWITCH_MODE can be 'dhcp' or 'static'
SWITCH_MODE=dhcp

# Set the interfaces up like in the interfaces files
# Usage: set_interfaces_up
set_interfaces_up()
{
set -x
    if [ "$SWITCH_MODE" = "auto" ]; then
	/sbin/ifup sw0ep
	return 0
    fi
    ip link set dev sw0ep up
    if [ "$SWITCH_MODE" = "dhcp" ]; then
        dhcpcd sw0ep &
    else
        ip addr add 192.168.102.114/22 dev sw0ep
        ip route add 192.168.100.0/22 dev sw0ep
    fi

    sleep 1

    # configure bridge
    ip link add name br0 type bridge
    ip link set dev br0 up
    ip link set dev sw0p1 master br0 up
    ip link set dev sw0p2 master br0 up
    ip link set dev sw0p3 master br0 up
}

# Set the interfaces down like in the interfaces files
# Usage: set_interfaces_down
set_interfaces_down()
{
set -x
    ip link set dev br0 down
    ip link delete dev br0

    ip link set dev sw0ep down
}

start()
{
    echo "[INFO]: ST set brigde interface"
    set_interfaces_up
}

stop() {
    set_interfaces_down
}

case "$1" in
    start)
        start
        ;;
    stop)
        stop
        ;;
    restart)
        stop
        start
        ;;
esac
exit 0
