PACKAGES:remove = "${PN}-stats"

do_install:append() {
    for f in ${FILES:nfs-utils-stats};do
	rm -vf ${D}${f}
    done
}
