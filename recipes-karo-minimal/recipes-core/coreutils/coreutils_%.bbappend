base_bindir_progs = "date"
bindir_progs = ""
sbindir_progs = ""

ALTERNATIVE_PRIORITY = "0"
ALTERNATIVE_PRIORITY[date] = "300"
ALTERNATIVE:${PN} = "date"
ALTERNATIVE:${PN}-doc = ""

do_install:append () {
    # make sure only explicitly wanted files are being installed
    echo "base_bindir = ${base_bindir}"
    echo "bindir      = ${bindir}"
    echo "sbindir     = ${sbindir}"
set -x
    tempdir=$(mktemp -d ${B}/tempdir-XXXXXX)
    for f in ${base_bindir_progs};do
        if [ "$base_bindir" != "$bindir" ];then
            mv -v ${D}${base_bindir}/$f.${BPN} $tempdir
        else
            mv -v ${D}${base_bindir}/$f $tempdir
        fi
    done
    rm -rvf "${D}/${base_bindir}"
    mv -v $tempdir "${D}${base_bindir}"

    if [ "$base_bindir" != "$bindir" ];then
        tempdir=$(mktemp -d ${B}/tempdir-XXXXXX)
        for f in ${bindir_progs};do
            mv -v ${D}${bindir}/$f $tempdir
        done
        rm -rvf "${D}${bindir}"
        mv -v $tempdir "${D}${bindir}"
    fi

    tempdir=$(mktemp -d ${B}/tempdir-XXXXXX)
    for f in ${sbindir_progs};do
        if [ "$sbindir" != "$bindir" ];then
            mv -v ${D}${sbindir}/$f.${BPN} $tempdir
        else
            mv -v ${D}${sbindir}/$f $tempdir
        fi
    done
    rm -rvf "${D}${sbindir}"
    mv -v $tempdir "${D}${sbindir}"

    # remove now empty directories
    echo "removing empty directories"
    find ${D} \( -path '*'${base_bindir} -o -path '*'${bindir} -o -path '*'${sbindir} \) -depth -type d -empty -exec rmdir -v \{\} \;
}
