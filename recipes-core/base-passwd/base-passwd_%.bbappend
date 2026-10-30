FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

# The oe-core patches switch root's login shell to /bin/sh and drop
# root's locked ("*") password entry (aimed at minimal images without
# bash and /etc/shadow). The Ka-Ro images always install bash and the
# shadow package, so keep the unpatched /etc/passwd entries.
SRC_URI:remove = "\
    file://0002-Use-bin-sh-instead-of-bin-bash-for-the-root-user.patch \
    file://0003-Remove-for-root-since-we-do-not-have-an-etc-shadow.patch \
"
