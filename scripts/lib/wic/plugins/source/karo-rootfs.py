#
# Copyright (c) 2014, Intel Corporation.
#
# SPDX-License-Identifier: GPL-2.0-only
#
# DESCRIPTION
# This implements the 'rootfs' source plugin class for 'wic'
# Added exporting of target rootfs as tar.bz
#
# AUTHORS
# Tom Zanussi <tom.zanussi (at] linux.intel.com>
# Joao Henrique Ferreira de Freitas <joaohf (at] gmail.com>
# Robin Leon Lintermann <rl (at] karo-electronics.de>
#

import os

from wic.misc import exec_native_cmd

from wic.plugins.source.rootfs import RootfsPlugin


class KaroRootfsPlugin(RootfsPlugin):
    name = 'karo-rootfs'

    @classmethod
    def __create_tar_bz2(cls, part, cr_workdir, native_sysroot, rootfs_dir):
        tar_name = "%s.tar.bz2" % part.label
        tar_path = os.path.realpath(os.path.join(cr_workdir, tar_name))

        if os.path.exists(tar_path):
            os.remove(tar_path)

        tar_cmd = "tar cjf %s -C %s ." % (tar_path, rootfs_dir)
        exec_native_cmd(tar_cmd, native_sysroot)

    @classmethod
    def do_prepare_partition(cls, part, source_params, cr, cr_workdir,
                             oe_builddir, bootimg_dir, kernel_dir,
                             rootfs_dir, native_sysroot):
        super().do_prepare_partition(
            part, source_params, cr, cr_workdir,
            oe_builddir, bootimg_dir, kernel_dir,
            rootfs_dir, native_sysroot
        )

        final_rootfs = part.rootfs_dir

        if part.exclude_path or part.include_path or part.change_directory or part.update_fstab_in_rootfs:
            final_rootfs = os.path.realpath(os.path.join(cr_workdir, "rootfs%d" % part.lineno))

        cls.__create_tar_bz2(part, cr_workdir, native_sysroot, final_rootfs)
