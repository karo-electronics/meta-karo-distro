SUMMARY = "A minimal Linux system without graphics support."

require karo-image.inc 
require karo-minimal.inc

python extend_recipe_sysroot:append() {
    if d.getVar('DISTRO') != 'karo-minimal':
        raise_sanity_error("cannot build '%s' with DISTRO '%s'" % (d.getVar('BPN'), d.getVar('DISTRO')), d)
}
