#
# SPDX-License-Identifier: GPL-2.0-only
#
# DESCRIPTION
# Shared tar export helpers for the karo wic source plugins
# (karo-rootfs, karo-bootimg-partition). Each partition's content is
# exported as tar archives corresponding to the tar.* entries in
# IMAGE_FSTYPES (e.g. tar.bz2, tar.gz, tar.xz, tar.zst). The compression
# is selected by the archive suffix via 'tar -a'. Archives are created
# per partition and deployed to IMGDEPLOYDIR as
# ${IMAGE_BASENAME}-${MACHINE}.<partition-label>.tar.<ending>.
# Note: IMAGE_FSTYPES must be added to WICVARS for the wic run to see it.
#

import os
import shutil

from wic.misc import exec_native_cmd, get_bitbake_var

SUPPORTED_TAR_FSTYPES = ['tar.bz2', 'tar.gz', 'tar.xz', 'tar.zst']


def canonicalize(paths):
    canonical_paths = []
    for path in paths:
        if "$" not in path:
            trailing_slash = path.endswith("/") and "/" or ""
            canonical_paths.append(os.path.realpath(path) + trailing_slash)
    return canonical_paths


def get_pseudo(native_sysroot, rootfs, pseudo_dir, ignore_paths):
    pseudo = "export PSEUDO_PREFIX=%s/usr;" % native_sysroot
    pseudo += "export PSEUDO_LOCALSTATEDIR=%s;" % pseudo_dir
    pseudo += "export PSEUDO_PASSWD=%s;" % rootfs
    pseudo += "export PSEUDO_NOSYMLINKEXP=1;"
    canonical_paths = canonicalize(ignore_paths)
    pseudo += "export PSEUDO_IGNORE_PATHS=%s;" % ','.join(canonical_paths)
    pseudo += "%s " % get_bitbake_var("FAKEROOTCMD")
    return pseudo


def create_tar(part_dir, tar_name, cr_workdir, native_sysroot, rootfs_dir, pseudo_dir, tar_opts=""):
    tar_path = os.path.realpath(os.path.join(cr_workdir, tar_name))
    if os.path.exists(tar_path):
        os.remove(tar_path)

    ignore_paths = [tar_path] + (get_bitbake_var("PSEUDO_IGNORE_PATHS") or "").split(',')
    pseudo = get_pseudo(native_sysroot, rootfs_dir, pseudo_dir, ignore_paths)
    opts = tar_opts and ("%s " % tar_opts) or ""
    tar_cmd = "tar -caf %s %s-C %s ." % (tar_path, opts, part_dir)
    exec_native_cmd(tar_cmd, native_sysroot, pseudo)
    return tar_path


def export_partition_tars(part_dir, part_label, cr_workdir, native_sysroot,
                          rootfs_dir, pseudo_dir, tar_opts=""):
    deploy_dir = get_bitbake_var("IMGDEPLOYDIR")
    dest_basename = "%s-%s" % (get_bitbake_var("IMAGE_BASENAME"), get_bitbake_var("MACHINE"))

    fstypes = [fstype for fstype in (get_bitbake_var("IMAGE_FSTYPES") or "").split() if fstype.startswith("tar")]
    for fstype in fstypes:
        if fstype not in SUPPORTED_TAR_FSTYPES:
            continue
        tar_name = f"{part_label}.{fstype}"
        tar_path = create_tar(part_dir, tar_name, cr_workdir, native_sysroot, rootfs_dir, pseudo_dir, tar_opts)
        shutil.copy2(tar_path, os.path.join(deploy_dir, f"{dest_basename}.{tar_name}"))
