do_install:append() {
    if ${@ bb.utils.contains('IMAGE_INSTALL','openssl',"false","true",d)};then
        rm -vf ${D}/${bindir}/rsync-ssl
    fi
}
