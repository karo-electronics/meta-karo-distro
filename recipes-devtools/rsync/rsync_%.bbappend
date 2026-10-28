do_install:append() {
    if ${@ bb.utils.contains('PACKAGECONFIG','openssl','false','true',d)};then
        rm -vf ${D}/${bindir}/rsync-ssl
    fi
}
