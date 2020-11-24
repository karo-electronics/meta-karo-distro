POKY_DISTRO ??= "poky"
require conf/distro/${POKY_DISTRO}.conf

POKY_DEFAULT_DISTRO_FEATURES_remove = "\
                                       largefile \
                                       opengl \
                                       ptest \
                                       vulkan \
                                       wayland \
"

DISTRO_FEATURES_DEFAULT_remove = "\
                                  3g \
                                  argp \
                                  bluetooth \
                                  nfc \
                                  pcmcia \
                                  wifi \
                                  x11 \
                                  zeroconf \
"

DISTRO_FEATURES_append = "\
                          alternatives-symlinks-relative \
                          dhcpcd \
                          ldconfig \
                          pam \
                          telnet-login \
"

VIRTUAL-RUNTIME_base-utils-hwclock = "util-linux-hwclock"
VIRTUAL-RUNTIME_dev_manager ?= "udev"
VIRTUAL-RUNTIME_init_manager ?= "${@bb.utils.contains('DISTRO_FEATURES',"systemd","systemd","sysvinit",d)}"

WARN_QA_remove = "host-user-contaminated"
