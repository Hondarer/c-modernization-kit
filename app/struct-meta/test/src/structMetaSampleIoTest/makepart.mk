TEST_SRCS := $(MYAPP_DIR)/prod/src/cmd/struct-meta-sample/struct_meta_sample.c
INCDIR += $(MYAPP_DIR)/prod/src/cmd/struct-meta-sample
LIBS += struct_meta cjson mock_cplat mock_libc
USE_WRAP_MAIN := 1
