FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

PACKAGECONFIG:append:stm32mp2 = "${@ bb.utils.contains('MACHINE_FEATURES', 'gpu', "", " use-pixman", d)}"

# GPU-less i.MX6 kernels: pixman renderer, not software GL.
PACKAGECONFIG:append:mx6-nxp-bsp = " use-pixman"

do_install:prepend() {
    # Replace template variables
    sed -i -e 's,@bindir@,${bindir},g' ${WORKDIR}/weston.ini
}

# Adwaita cursors, installed by karo-image-weston on mx6
do_install:append:mx6-nxp-bsp() {
    sed -i -e 's/^#\[shell\]$/[shell]/' -e '/^\[shell\]$/a cursor-theme=Adwaita' \
        ${D}${sysconfdir}/xdg/weston/weston.ini
}
