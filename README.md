# meta-karo-distro

Distro Layer for the Ka-Ro Yocto BSP for Ka-Ro TX and QS CoMs.

For documentation see our Yocto Guide: https://karo-electronics.github.io/docs/yocto-guide/index.html

## Distros

* `karo-minimal`  - minimal system with read-only rootfs, no graphics
* `karo-wayland`  - Wayland backend
* `karo-xwayland` - Wayland with X11/Xwayland backend

## Dependencies

* `openembedded-core` (layer series `wrynose`, Yocto Project 6.0)
* `karo` (mainline BSP) or `karo-nxp` (NXP BSP), selected via
  `LAYERDEPENDS` and the `use-mainline-bsp` / `use-nxp-bsp` overrides
* optional, when present in `BBLAYERS`:
  `meta-imx` / `meta-freescale` (bbappend and BBMASK interaction, see
  `conf/layer.conf`), and `meta-imx-ml` (collection
  `imx-machine-learning`, pulled in through `BBFILES_DYNAMIC`)

The init manager follows the standard `INIT_MANAGER` variable (derived
from `DISTRO_FEATURES`; the `karo-distro/systemd` fragment switches to
systemd). All images, including `karo-image-weston`, build and run with
both sysvinit and systemd, and with a read-only root filesystem
(`karo-distro/read-only-rootfs` fragment) or a writable one.

## Layer configuration variables

Custom variables defined by this layer (see the referencing files for
details):

* `NETWORK_INTERFACE_PREFIX` - network interface name prefix seen on
  the target (`conf/distro/include/karo-distro.inc`)
* `NETWORK_INTERFACES` - interfaces configured by init-ifupdown
* `NETWORK_INTERFACES_AUTO` - subset brought up automatically
* `PRIMARY_NETWORK_INTERFACE` - optional; the interface whose DHCP
  state seeds `/etc/ntp.conf` and `/etc/resolv.conf`
  (`recipes-connectivity/dhcpcd`); must be listed in
  `NETWORK_INTERFACES`
* `KARO_PACKAGING_VENDOR` - flash packaging vendor, selects
  `recipes-karo/images/packaging/karo-<vendor>.inc`
* `WIC_PARTITIONS_EXPORT` - wic partitions exported as standalone
  filesystem images (`classes/export-wic-partitions.bbclass`)
* `BOOTFS_PARTITION_SIZE`, `ROOTFS_PARTITION_SIZE`,
  `USERFS_PARTITION_SIZE` - image partition sizes in KiB
  (`recipes-karo/images/karo-image.inc`)

## License

The layer metadata is released under the MIT license, see the LICENSE
file.

---------
(c) 2020 - Ka-Ro electronics GmbH - www.karo-electronics.com
