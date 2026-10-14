# Export WIC partition images to the deploy directory
#
# This class provides the do_export_wic_partitions task, which runs after
# do_image_wic and copies the individual partition filesystem images from
# the wic build directory (${WORKDIR}/build-wic) into ${IMGDEPLOYDIR},
# so they can be flashed standalone without writing the full disk image.
#
# WIC_PARTITIONS_EXPORT controls which partitions are exported. It is a
# space-separated list of partition filesystem labels, as assigned with
# '--label <name>' in the wks file. Only partitions whose label appears
# in this list are exported; with the default empty value nothing is
# exported and the task is effectively a no-op.
#
#   WIC_PARTITIONS_EXPORT = "boot rootfs userfs"
#
# Each partition image in the build directory is inspected with blkid to
# determine its filesystem label (LABEL) and type (TYPE). A partition is
# exported as:
#
#   ${IMAGE_BASENAME}-${MACHINE}.<label>.<fstype>
#
# e.g. myimage-mymachine.boot.ext4. The <fstype> suffix is the filesystem
# type reported by blkid (e.g. ext4, vfat), not the wic fstype. Partitions
# whose filesystem carries no label are never matched by this list.
#
# This class is typically inherited together with image_types_wic, e.g.
# from a wic-specific include file.

WIC_PARTITIONS_EXPORT ?= ""

python do_export_wic_partitions() {
    import subprocess

    def get_partition_meta(path):
        out = subprocess.run(
            ["blkid", "-o", "export", path], text=True, capture_output=True
        ).stdout.splitlines()
        data = dict(line.split("=", 1) for line in out)
        return data.get("LABEL", "img"), data.get("TYPE", "bin")

    build_wic = os.path.join(d.getVar("WORKDIR"), "build-wic")
    deploy_dir = d.getVar("IMGDEPLOYDIR")
    image_basename = d.getVar("IMAGE_BASENAME")
    machine = d.getVar("MACHINE")
    export_partitions = d.getVar("WIC_PARTITIONS_EXPORT").split()

    for fname in os.listdir(build_wic):
        if ".direct.p" not in fname:
            continue
        path = os.path.join(build_wic, fname)
        fsname, fstype = get_partition_meta(path)
        if fsname not in export_partitions:
            continue
        dest_basename = f"{image_basename}-{machine}.{fsname}"
        bb.utils.copyfile(path, os.path.join(deploy_dir, f"{dest_basename}.{fstype}"))
}
addtask export_wic_partitions after do_image_wic before do_image_complete
