# The nanopb provided Makefile rules
include nanopb/extra/nanopb.mk

CC = gcc

CFLAGS = -Wall -Werror -g -O0
CFLAGS += -I$(NANOPB_DIR)

SRC  = \
c_example.c \
sample.pb.c \
$(NANOPB_DIR)/pb_encode.c \
$(NANOPB_DIR)/pb_decode.c \
$(NANOPB_DIR)/pb_common.c

simple: $(CSRC)
	$(CC) $(CFLAGS) -oc_example $(SRC)

simple.pb.c: simple.proto
	make -BC nanopb/generator/proto
	$(PROTOC) --python_out=. sample.proto
	$(PROTOC) $(PROTOC_OPTS) --nanopb_out=. sample.proto