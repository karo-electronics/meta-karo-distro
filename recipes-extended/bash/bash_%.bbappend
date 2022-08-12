FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
SRC_URI:append = " \
	       	  file://bash_aliases\
	       	  file://bash_login\
"
FILES:${PN} += " \
	       ${ROOT_HOME}/.bash_aliases \
	       ${ROOT_HOME}/.bash_login \
"

do_install:append() {
    install -d -m 700 ${D}${ROOT_HOME}
    install -m 644 ${WORKDIR}/bash_aliases ${D}${ROOT_HOME}/.bash_aliases
    install -m 644 ${WORKDIR}/bash_login ${D}${ROOT_HOME}/.bash_login
}
