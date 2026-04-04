OUTPUT_DIR := $(WORKSPACE_FOLDER)/prod/util/lib
ifneq ($(OS),Windows_NT)
    LDFLAGS += -ldl -lpthread
endif
