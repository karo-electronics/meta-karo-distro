# The IMX219 on the QS95 goes through the NEO ISP: try the nxp/neo pipeline handler before the raw imx8-isi one.
LIBCAMERA_PIPELINES:mx95-nxp-bsp = "nxp/neo,imx8-isi,mali-c55,simple,uvcvideo"
