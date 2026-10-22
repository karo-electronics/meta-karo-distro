#
# Copyright (c) 2014, Intel Corporation.
#
# SPDX-License-Identifier: GPL-2.0-only
#
# DESCRIPTION
# This implements the 'rootfs' source plugin class for 'wic'.
# In addition to the standard rootfs partition handling, it exports each
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
# Tom Zanussi <tom.zanussi (at] linux.intel.com>
# Joao Henrique Ferreira de Freitas <joaohf (at] gmail.com>
# Robin Leon Lintermann <rl (at] karo-electronics.de>
#

import importlib.util
import os

from wic.misc import get_bitbake_var
from wic.plugins.source.rootfs import RootfsPlugin


def _load_karo_tar():
    path = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'karo_tar.py')
    spec = importlib.util.spec_from_file_location('karo_tar', path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


karo_tar = _load_karo_tar()


class KaroRootfsPlugin(RootfsPlugin):
    name = 'karo-rootfs'

    @classmethod
    def do_prepare_partition(cls, part, source_params, cr, cr_workdir,
                             oe_builddir, bootimg_dir, kernel_dir,
                             krootfs_dir, native_sysroot):
        super().do_prepare_partition(
            part, source_params, cr, cr_workdir,
            oe_builddir, bootimg_dir, kernel_dir,
            krootfs_dir, native_sysroot
        )

        part_dir = part.rootfs_dir
        rootfs_dir = os.path.realpath(get_bitbake_var("IMAGE_ROOTFS"))
        pseudo_dir = os.path.join(rootfs_dir, "../pseudo")

        if part.exclude_path or part.include_path or part.change_directory or part.update_fstab_in_rootfs:
            part_dir = os.path.realpath(os.path.join(cr_workdir, "rootfs%d" % part.lineno))
            pseudo_dir = os.path.realpath(os.path.join(cr_workdir, "pseudo%d" % part.lineno))

        karo_tar.export_partition_tars(part_dir, part.label, cr_workdir,
                                       native_sysroot, rootfs_dir, pseudo_dir)
