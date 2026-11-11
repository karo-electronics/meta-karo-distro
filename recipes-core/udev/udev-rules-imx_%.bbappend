# The upstream 10-imx.rules renames the linux,cma* device node
# (NAME="dma_heap"). udev cannot rename device nodes created by the
# kernel itself, so the NAME= assignment is ignored with a warning at
# every boot:
#   udevd: NAME="dma_heap" ignored, kernel device nodes can not be renamed;
#          please fix it in /etc/udev/rules.d/10-imx.rules:24
# The DMA-BUF heap nodes under /dev/dma_heap/ are already covered by
# 50-weston.rules (SUBSYSTEM=="dma_heap", GROUP="weston", MODE="0660").
# Drop the rename and keep the permissions on the legacy linux,cma node
# (needed on SoCs where the CMA heap is exposed as a misc node instead
# of a dma_heap class device).
do_install:append() {
    rules=${D}${sysconfdir}/udev/rules.d/10-imx.rules
    sed -i -e 's/^KERNEL=="linux,cma\*", NAME="dma_heap", /KERNEL=="linux,cma*", /' $rules
}
