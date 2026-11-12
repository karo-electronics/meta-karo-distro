SUMMARY = "Bootloader flash package: U-Boot/bootloader binaries and flash scripts only, no Linux image"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

inherit nopackages

PACKAGE_ARCH = "${MACHINE_ARCH}"

# Ka-Ro specific flash package generation
require packaging/karo.inc

# The factory package contains bootloader binaries and flash scripts
# only, no Linux image is built here, so only flash the bootloader
# and erase the GPT partition table.
KARO_UUU_FLASH_STEPS = "gpt erase bootloader"
KARO_UUU_FLASH_STEPS:remove = "${@bb.utils.contains('MACHINE_FEATURES', 'nand', 'gpt', '', d)}"
KARO_UUU_FLASH_CMD_gpt = "FB: ${KARO_UUU_UCMD}gpt write mmc 0 'start=1MiB,name=data,size=2MiB;'"
KARO_UUU_FLASH_CMD_erase = "${@bb.utils.contains('MACHINE_FEATURES', 'emmc', "FB: erase data", "FB: erase rootfs", d)}"
KARO_FLASH_FIX_GPT = "0"

# For STM32MP, parts of the bootloader are flashed to the eMMC user area.
KARO_UUU_FLASH_STEPS:remove:stm32mpcommon = "erase"
KARO_UUU_FLASH_STEPS:append:stm32mpcommon = " fip"
KARO_UUU_FLASH_CMD_gpt:stm32mpcommon = "FB: oem ucmd:gpt write mmc 0 'start=1MiB,name=fip,type=19d5df83-11b0-457b-be2c-7559c13142a5,size=2MiB;'"
KARO_UUU_FLASH_CMD_fip:stm32mpcommon = "FB: flash fip fip-${BOOTLOADER_BASENAME}.bin"

# Bootloader recipes that make up the package. Their recipe SBOMs (SPDX)
# are merged into the combined package SBOM below.
KARO_BOOTLOADER_SBOM_RECIPES:use-nxp-bsp = "\
    virtual/bootloader \
    imx-atf \
    ${IMX_EXTRA_FIRMWARE} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'optee', 'optee-os', '', d)} \
"
KARO_BOOTLOADER_SBOM_RECIPES:rzg2 = "virtual/bootloader tf-a-rzg2 flashwriter-rzg2"
KARO_BOOTLOADER_SBOM_RECIPES:stm32mpcommon = "\
    virtual/bootloader \
    tf-a-stm32mp \
    ${@bb.utils.contains('MACHINE_FEATURES', 'optee', 'optee-os-stm32mp', '', d)} \
"

# i.MX6 boots a monolithic DCD U-Boot image: no ATF
# (mirrors DEPENDS:remove:mx6-nxp-bsp in u-boot-karo).
KARO_BOOTLOADER_SBOM_RECIPES:remove:mx6-nxp-bsp = "imx-atf"

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
            d, "recipes", "recipe-%s" % pn, oe.spdx30.build_Build)
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
do_create_image_sbom_spdx[file-checksums] += "${SPDX3_LIB_DEP_FILES}"

python do_create_image_sbom_spdx_setscene() {
    sstate_setscene(d)
}
addtask do_create_image_sbom_spdx_setscene
