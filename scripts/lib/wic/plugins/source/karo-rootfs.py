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

from wic.misc import exec_native_cmd, get_bitbake_var

from wic.plugins.source.rootfs import RootfsPlugin


class KaroRootfsPlugin(RootfsPlugin):
    name = 'karo-rootfs'

    @staticmethod
    def __canonicalize(paths):
        canonical_paths = []
        for path in paths:
            if "$" not in path:
                trailing_slash = path.endswith("/") and "/" or ""
                canonical_paths.append(os.path.realpath(path) + trailing_slash)
        return canonical_paths

    @staticmethod
    def __get_pseudo(native_sysroot, rootfs, pseudo_dir, ignore_paths):
        canonical_paths = KaroRootfsPlugin.__canonicalize(ignore_paths)
        pseudo = "export PSEUDO_IGNORE_PATHS=%s;" % ','.join(canonical_paths)
        pseudo += RootfsPlugin._RootfsPlugin__get_pseudo(native_sysroot, rootfs, pseudo_dir)
        return pseudo

    @classmethod
    def __create_tar_bz2(cls, part, cr_workdir, native_sysroot, part_dir, rootfs_dir, pseudo_dir):
        tar_name = "%s.tar.bz2" % part.label
        tar_path = os.path.realpath(os.path.join(cr_workdir, tar_name))

        if os.path.exists(tar_path):
            os.remove(tar_path)

        ignore_paths = [tar_path] + (get_bitbake_var("PSEUDO_IGNORE_PATHS") or "").split(',')
        pseudo = cls.__get_pseudo(native_sysroot, rootfs_dir, pseudo_dir, ignore_paths)
        tar_cmd = "tar cjf %s -C %s ." % (tar_path, part_dir)
        exec_native_cmd(tar_cmd, native_sysroot, pseudo)

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

        cls.__create_tar_bz2(part, cr_workdir, native_sysroot, part_dir, rootfs_dir, pseudo_dir)
