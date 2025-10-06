inherit relative_symlinks

dirs755:remove = " \
    ${localstatedir}/volatile \
    ${localstatedir}/volatile/log \
"

volatiles = ""
