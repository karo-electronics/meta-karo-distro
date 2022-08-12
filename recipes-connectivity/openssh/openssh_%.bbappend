inherit relative_symlinks

RRECOMMENDS:${PN}-sshd:remove:class-target = " rng-tools"
RRECOMMENDS:${PN}-sshd:append:class-target = " haveged"
