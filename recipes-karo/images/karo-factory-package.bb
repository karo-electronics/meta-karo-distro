SUMMARY = "Bootloader flash package: U-Boot/bootloader binaries and flash scripts only, no Linux image"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

inherit nopackages

PACKAGE_ARCH = "${MACHINE_ARCH}"

# Vendor specific flash package generation (see packaging/karo.inc):
# packaging/karo.inc carries the common flash package logic and the
# vendor selection helper; the second require resolves to
# packaging/karo-<vendor>.inc for the BSP vendor set in
# KARO_PACKAGING_VENDOR.
require packaging/karo.inc
require ${@karo_packaging_vendor_inc(d)}

# The factory package contains bootloader binaries and flash scripts
# only, no Linux image is built here, so only flash the bootloader.
KARO_UUU_FLASH_STEPS = "bootloader"

# Bootloader recipes that make up the package. Their recipe SBOMs (SPDX)
# are merged into the combined package SBOM below.
KARO_BOOTLOADER_SBOM_RECIPES:use-nxp-bsp = "\
    virtual/bootloader \
    imx-atf \
    ${IMX_EXTRA_FIRMWARE} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'optee', 'optee-os', '', d)} \
"

# The bootloader recipes must be in the dependency closure so their SBOMs
# are built before the combined package SBOM is generated.
DEPENDS += "${KARO_BOOTLOADER_SBOM_RECIPES}"

# Generate a single combined SBOM (SPDX) for the bootloader components in
# the package, following the pattern of do_create_image_sbom_spdx from
# create-spdx-image-3.0.bbclass: a dedicated sstate task writes the
# document to a staging directory, which is deployed to DEPLOY_DIR_IMAGE
# via the sstate mechanism.
SPDXIMAGEDEPLOYDIR = "${SPDXDIR}/image-deploy"

python do_create_image_sbom_spdx() {
    import oe.sbom30
    import oe.spdx30

    recipes = (d.getVar('KARO_BOOTLOADER_SBOM_RECIPES') or "").split()
    if not recipes:
        return

    root_elements = []
    dep_objsets = []
    for r in recipes:
        if r.startswith('virtual/'):
            pn = d.getVar('PREFERRED_PROVIDER_%s' % r)
            if not pn:
                bb.fatal('No PREFERRED_PROVIDER for %s' % r)
        else:
            pn = r
        dep_build, dep_objset = oe.sbom30.find_root_obj_in_jsonld(
            d, "builds", "build-%s" % pn, oe.spdx30.build_Build)
        root_elements.append(dep_build)
        dep_objsets.append(dep_objset)

    name = "%s-%s" % (d.getVar('BPN'), d.getVar('MACHINE'))
    objset, sbom = oe.sbom30.create_sbom(d, name, root_elements,
                                         add_objectsets=dep_objsets)

    from pathlib import Path
    oe.sbom30.write_jsonld_doc(d, objset,
        Path(d.getVar('SPDXIMAGEDEPLOYDIR')) / ("%s.spdx.json" % name))
}
addtask do_create_image_sbom_spdx after do_create_spdx before do_build
SSTATETASKS += "do_create_image_sbom_spdx"
do_create_image_sbom_spdx[sstate-inputdirs] = "${SPDXIMAGEDEPLOYDIR}"
do_create_image_sbom_spdx[sstate-outputdirs] = "${DEPLOY_DIR_IMAGE}"
do_create_image_sbom_spdx[recrdeptask] += "do_create_spdx"
do_create_image_sbom_spdx[cleandirs] = "${SPDXIMAGEDEPLOYDIR}"
do_create_image_sbom_spdx[stamp-extra-info] = "${MACHINE_ARCH}"
do_create_image_sbom_spdx[file-checksums] += "${SPDX3_DEP_FILES}"

python do_create_image_sbom_spdx_setscene() {
    sstate_setscene(d)
}
addtask do_create_image_sbom_spdx_setscene
