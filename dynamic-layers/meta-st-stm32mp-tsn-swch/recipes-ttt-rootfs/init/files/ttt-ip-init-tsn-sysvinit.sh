#!/bin/sh
set -x

# Set the interfaces up like in the interfaces files
# Usage: set_interfaces_up
set_interfaces_up()
{
    # ask to network to put an ip address on this interface
    # udhcpc -i sw0ep > /dev/null 2>&1 &
    # ip addr add 192.168.0.10 dev sw0ep
    # ip route add 192.168.0.0/24 dev sw0ep

    #sleep 1

    ip link add name br0 type bridge
    ip link set dev br0 up
    ip link set dev sw0p1 master br0 up
    ip link set dev sw0p2 master br0 up
    ip link set dev sw0p3 master br0 up
    ip link set dev sw0ep up
}

# Set the interfaces down like in the interfaces files
# Usage: set_interfaces_down
set_interfaces_down()
{
    ip link set dev br0 down
    ip link delete dev br0

    ip link set dev sw0ep down
}

# Start the deamons as they would do at start
# Usage: start_daemons
start_daemons()
{
    ip link set br0 type bridge stp_state 1

    mstpd -d -v 2 &
    
    mstpctl addbridge br0
    mstpctl setforcevers br0 mstp
    mstpctl setvid2fid br0 0:1

    /etc/init.d/lldpd start &

    /etc/init.d/deptp start &

    #/etc/init.d/snmpd start &

    #/usr/share/netopeer2-server/netopeer2-server-service start &
}

# Stop the daemons
# Usage: stop_daemons
stop_daemons()
{
    mstpctl delbridge br0
    #/etc/init.d/snmpd stop &
    /etc/init.d/lldpd stop &
    /etc/init.d/deptp stop &
    #/usr/share/netopeer2-server/netopeer2-server-service stop
    killall mstpd
}

start()
{
    echo "[INFO]: ST set brigde interface"
    set_interfaces_up
    echo "[INFO]: start service"
    start_daemons
}

stop() {
    stop_daemons
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
    restore)
        /usr/share/netopeer2-server/netopeer2-server-service restore
        ;;
esac
exit 0
