# mkubifs parameters
MKUBIFS_LEB_SIZE = "126976"

MKUBIFS_ARGS ?= "-F -x zlib -m 2048 -e ${MKUBIFS_LEB_SIZE}"

UBINIZE_ARGS ?= "-m 2048 -s 2048 -p 128KiB"
