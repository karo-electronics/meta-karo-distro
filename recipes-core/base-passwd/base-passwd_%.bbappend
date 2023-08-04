FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI:remove = "\
        file://0002-Use-bin-sh-instead-of-bin-bash-for-the-root-user.patch \
        file://0003-Remove-for-root-since-we-do-not-have-an-etc-shadow.patch \
"
