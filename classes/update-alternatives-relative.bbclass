#
# Copyright OpenEmbedded Contributors
#
# SPDX-License-Identifier: MIT
#

# This class rewrites the generated update-alternatives calls so that
# symlink targets are stored as relative paths instead of absolute paths.
#
# update-alternatives.bbclass generates postinst/prerm scripts
# which are executed at install time.
# Therefore we need to rewrite the generated update-alternatives commands.

PACKAGESPLITFUNCS:append = " update_alternatives_symlinks_relative"

python update_alternatives_symlinks_relative () {
    if not bb.data.inherits_class('update-alternatives', d):
        return

    if not update_alternatives_enabled(d):
        return

    import os
    import re

    def rel_target(link, target):
        if not os.path.isabs(target):
            return target
        return os.path.relpath(target, os.path.dirname(link))

    install_rx = re.compile(
        r'^(?P<prefix>\s*update-alternatives\s+--install\s+)'
        r'(?P<link>\S+)\s+(?P<name>\S+)\s+(?P<target>\S+)\s+(?P<priority>\S+)',
        re.MULTILINE)

    remove_rx = re.compile(
        r'^(?P<prefix>\s*update-alternatives\s+--remove\s+)'
        r'(?P<name>\S+)\s+(?P<target>\S+)',
        re.MULTILINE)

    for pkg in (d.getVar('PACKAGES') or "").split():
        postinst = d.getVar('pkg_postinst:%s' % pkg) or ''
        prerm = d.getVar('pkg_prerm:%s' % pkg) or ''

        target_map = {}

        def install_repl(match):
            link = match.group('link')
            name = match.group('name')
            target = match.group('target')
            priority = match.group('priority')
            new_target = rel_target(link, target)
            target_map[(name, target)] = new_target
            return '%s%s %s %s %s' % (
                match.group('prefix'), link, name, new_target, priority)

        def remove_repl(match):
            name = match.group('name')
            target = match.group('target')
            new_target = target_map.get((name, target), target)
            return '%s%s %s' % (match.group('prefix'), name, new_target)

        if postinst:
            postinst = install_rx.sub(install_repl, postinst)
            d.setVar('pkg_postinst:%s' % pkg, postinst)

        if prerm and target_map:
            prerm = remove_rx.sub(remove_repl, prerm)
            d.setVar('pkg_prerm:%s' % pkg, prerm)
}
