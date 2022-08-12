PACKAGECONFIG:remove = "${@bb.utils.contains('DISTRO_FEATURES', 'wifi', '', 'cryptodev-linux',d)}"
