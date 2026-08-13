# Export WIC partitions to deploy directory
# 
# This class provides the do_export_wic_partitions task which exports
# partition images from the WIC build directory to the deploy directory.
#
# The partitions to export are controlled by the WIC_PARTITIONS_EXPORT variable,
# which should be a space-separated list of partition labels.

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
        path = os.path.join(build_wic, f"{fsname}.tar.bz2")
        bb.utils.copyfile(path, os.path.join(deploy_dir, f"{dest_basename}.tar.bz2"))
}
addtask export_wic_partitions after do_image_wic before do_image_complete
