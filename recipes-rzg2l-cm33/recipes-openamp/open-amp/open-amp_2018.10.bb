FILESEXTRAPATHS:prepend := "${THISDIR}/2018.10:"
SRCBRANCH ?= "master"
SRCREV ?= "c5763fc0950455cb72c7d452bc1115f6904b6ca0"
LIC_FILES_CHKSUM ?= "file://LICENSE.md;md5=a8d8cf662ef6bf9936a1e1413585ecbf"

SRC_URI:append = " \
  file://0001-fix-allocate-id.patch \
  file://0001_fix_error_return_of_virtio_create_virtqueues.patch \
  file://0002_fix_error_return_of_remoteproc_remove.patch \
  file://0003_fix_compilation_error_of_virtqueue.patch \
  file://0004_rpmsg_send_do_not_check_buffer_size_when_get_buffer_failed.patch \
  file://0005_rpmsg_virtio_fix_get_buffer_size_return.patch \
  file://0006_rpmsg_virtio_limit_the_buffer_allocate_from_shared_memory_pool.patch \
  file://0007-Remove-nested-structs-in-header.patch\
  "

include open-amp.inc
