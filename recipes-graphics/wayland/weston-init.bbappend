FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

# Ka-Ro default Weston configuration (replaces the upstream
# weston.ini); the init script delta (start Weston in runlevel 2 only)
# is expressed as a patch so it surfaces on oe-core upgrades.
SRC_URI += " \
    file://0001-weston-init-start-Weston-in-runlevel-2-only.patch \
"

do_install:prepend() {
    # Replace template variables
    sed -i -e 's,@bindir@,${bindir},g' ${UNPACKDIR}/weston.ini
}
