# Shadow the weston recipe's xwayland.weston-start through FILESEXTRAPATHS:
# the Ka-Ro copy additionally creates /tmp/.X11-unix with mode 3777, the
# directory Xwayland expects for its sockets. /tmp is a symlink to the
# volatile /var/tmp, so this also works on read-only-rootfs images.
FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"
