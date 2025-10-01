PREFERRED_PROVIDER_opencl-headers = "imx-gpu-viv"

PREFERRED_VERSION_gstreamer1.0:karo-wayland = "1.22.0.imx"
PREFERRED_VERSION_gstreamer1.0-plugins-base:karo-wayland = "1.22.0.imx"
PREFERRED_VERSION_gstreamer1.0-plugins-good:karo-wayland = "1.22.0.imx"
PREFERRED_VERSION_gstreamer1.0-plugins-bad:karo-wayland = "1.22.0.imx"
PREFERRED_VERSION_gstreamer1.0-rtsp-server:karo-wayland = "1.22.0.imx"

DISTRO_FEATURES_DEFAULT:append:mx8-nxp-bsp  = " dsi83"
DISTRO_FEATURES_DEFAULT:append:mx8mp-nxp-bsp  = " flexcan"
DISTRO_FEATURES_DEFAULT:append:tx8m = " pcie"
DISTRO_FEATURES_DEFAULT:append:qs8m = " csi-camera imx219 raspi-display tc358867"
DISTRO_FEATURES_DEFAULT:append:qs93 = " csi-camera imx219"
DISTRO_FEATURES_DEFAULT:append:qsxm = " basler csi-camera imx219 pcie raspi-display tc358867"
DISTRO_FEATURES_DEFAULT:append:qsxp = " basler csi-camera pcie raspi-display tc358867"
DISTRO_FEATURES_DEFAULT:append:tx8p = " lvds pcie"

# mask out sysvinit recipes from meta-imx layer
BBMASK:append = " .*/meta-imx/.*/sysvinit/.*"
# mask out silly weston-init.bbappend from meta-imx layer
BBMASK:append = " .*/meta-imx/.*/weston-init.bbappend"
