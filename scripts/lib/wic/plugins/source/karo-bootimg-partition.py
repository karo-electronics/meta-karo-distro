#
# Copyright (c) 2014, Intel Corporation.
#
# SPDX-License-Identifier: GPL-2.0-only
#
# DESCRIPTION
# This implements the 'bootimg-partition' source plugin class for 'wic'.
# In addition to the standard boot partition handling, it exports each
# partition's content as tar archives corresponding to the tar.* entries in
# IMAGE_FSTYPES (e.g. tar.bz2, tar.gz, tar.xz, tar.zst). The compression is
# selected by the archive suffix via 'tar -a'. Archives are created per
# partition and deployed to IMGDEPLOYDIR as
# ${IMAGE_BASENAME}-${MACHINE}.<partition-label>.tar.<ending>.
# Note: IMAGE_FSTYPES must be added to WICVARS for the wic run to see it.
# The tar export logic is shared and lives in
# plugins/karo_tar.py.
#
# AUTHORS
# Maciej Borzecki <maciej.borzecki (at] open-rnd.pl>
# Robin Leon Lintermann <rl (at] karo-electronics.de>
#

import importlib
import importlib.util
import os

from wic.misc import get_bitbake_var

# 'bootimg-partition' contains a hyphen and therefore cannot appear in a
# regular import statement; importlib.import_module has no such restriction.
BootimgPartitionPlugin = importlib.import_module(
    "wic.plugins.source.bootimg-partition").BootimgPartitionPlugin


def _load_karo_tar():
    path = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'karo_tar.py')
    spec = importlib.util.spec_from_file_location('karo_tar', path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


karo_tar = _load_karo_tar()


class KaroBootimgPartitionPlugin(BootimgPartitionPlugin):
    """
    Create an image of boot partition, copying over files
    listed in IMAGE_BOOT_FILES bitbake variable and exporting
    the partition content as tar archives.
    """

    name = 'karo-bootimg-partition'

    @classmethod
    def do_prepare_partition(cls, part, source_params, cr, cr_workdir,
                             oe_builddir, bootimg_dir, kernel_dir,
                             rootfs_dir, native_sysroot):
        super().do_prepare_partition(
            part, source_params, cr, cr_workdir,
            oe_builddir, bootimg_dir, kernel_dir,
            rootfs_dir, native_sysroot
        )

        part_dir = "%s/boot.%d" % (cr_workdir, part.lineno)
        rootfs_dir = os.path.realpath(get_bitbake_var("IMAGE_ROOTFS"))
        pseudo_dir = os.path.join(rootfs_dir, "../pseudo")

        karo_tar.export_partition_tars(part_dir, part.label, cr_workdir,
                                       native_sysroot, rootfs_dir, pseudo_dir,
                                       tar_opts="--owner=root --group=root")
