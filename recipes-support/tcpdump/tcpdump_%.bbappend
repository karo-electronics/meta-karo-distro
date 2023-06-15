PACKAGECONFIG:remove = "${@ bb.utils.contains('IMAGE_INSTALL','openssl',"","openssl",d)}"
