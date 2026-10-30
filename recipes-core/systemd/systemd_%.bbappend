FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

# The systemd recipe installs every *.rules file from SRC_URI into
# ${sysconfdir}/udev/rules.d (its do_install globs ${UNPACKDIR} for
# *.rules), so extending SRC_URI is sufficient to ship the rules.
SRC_URI:append = "\
        file://50-firmware.rules \
"
