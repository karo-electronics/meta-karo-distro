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
    def __get_pseudo(native_sysroot, pseudo_dir, rootfs_dir, tar_path):
        # Same environment as wic's Partition.prepare_rootfs()
        ignore_paths = [tar_path] + (get_bitbake_var("PSEUDO_IGNORE_PATHS") or "").split(",")
        canonical_paths = []
        for path in ignore_paths:
            if "$" not in path:
                trailing_slash = path.endswith("/") and "/" or ""
                canonical_paths.append(os.path.realpath(path) + trailing_slash)

        # /etc/passwd lives in the image rootfs, not in /boot
        passwd_dir = get_bitbake_var("IMAGE_ROOTFS") or rootfs_dir
        pseudo = "export PSEUDO_PREFIX=%s/usr;" % native_sysroot
        pseudo += "export PSEUDO_LOCALSTATEDIR=%s;" % pseudo_dir
        pseudo += "export PSEUDO_PASSWD=%s;" % os.path.realpath(passwd_dir)
        pseudo += "export PSEUDO_NOSYMLINKEXP=1;"
        pseudo += "export PSEUDO_IGNORE_PATHS=%s;" % ",".join(canonical_paths)
        pseudo += "%s " % get_bitbake_var("FAKEROOTCMD")
        return pseudo

    @staticmethod
    def __find_pseudo_dir(part, cr_workdir, copied):
        # Pseudo database lookup order of wic's RootfsPlugin
        if copied:
            candidates = [os.path.join(cr_workdir, "pseudo%d" % part.lineno)]
        else:
            candidates = [os.path.join(part.rootfs_dir, "../pseudo")]
            image_rootfs = get_bitbake_var("IMAGE_ROOTFS")
            if image_rootfs:
                candidates.append(os.path.join(image_rootfs, "../pseudo"))
        for pseudo_dir in candidates:
            if os.path.lexists(pseudo_dir):
                return os.path.realpath(pseudo_dir)
        return None

    @classmethod
    def __create_tar_bz2(cls, part, cr_workdir, native_sysroot, rootfs_dir,
                         pseudo_dir):
        tar_name = "%s.tar.bz2" % part.label
        tar_path = os.path.realpath(os.path.join(cr_workdir, tar_name))

        if os.path.exists(tar_path):
            os.remove(tar_path)

        pseudo = None
        if pseudo_dir:
            pseudo = cls.__get_pseudo(native_sysroot, pseudo_dir, rootfs_dir,
                                      tar_path)

        tar_cmd = "tar cjf %s -C %s ." % (tar_path, rootfs_dir)
        exec_native_cmd(tar_cmd, native_sysroot, pseudo)

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
        copied = False

        if part.exclude_path or part.include_path or part.change_directory or part.update_fstab_in_rootfs:
            final_rootfs = os.path.realpath(os.path.join(cr_workdir, "rootfs%d" % part.lineno))
            copied = True

        pseudo_dir = cls.__find_pseudo_dir(part, cr_workdir, copied)
        cls.__create_tar_bz2(part, cr_workdir, native_sysroot, final_rootfs,
                             pseudo_dir)
