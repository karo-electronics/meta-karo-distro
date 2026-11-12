# packagegroup-qt6-imx pulls in packagegroup-imx-gopoint on 'x11 wayland'
# distro features, but no recipe in this layer set provides it
RDEPENDS:${PN}:remove = "packagegroup-imx-gopoint"
