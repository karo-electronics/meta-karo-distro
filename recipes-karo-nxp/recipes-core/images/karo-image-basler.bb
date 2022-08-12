SUMMARY = "An image with full multimedia, Machine Learning and Basler camera support"

require karo-image-ml.bb

REQUIRED_DISTRO_FEATURES = "x11 wayland"

IMAGE_INSTALL:append = " \
        kernel-module-basler-camera \
        pylon \
        python3-pypylon \
        gentl-producer \
        v4l-utils \
        xauth \
        imx-vpu-hantro-daemon \
        packagegroup-fsl-gstreamer1.0 \
        packagegroup-fsl-gstreamer1.0-full \
"

IMAGE_INSTALL:append:mx8mp-nxp-bsp = " \
        kernel-module-isp-vvcam \
        imx8mp-modprobe-config \
        basler-camera \
        isp-imx \
        packagegroup-imx-isp \
"
