# インクルードパス
INCDIR += \
	$(TESTFW_DIR)/gtest/include \
	$(TESTFW_DIR)/include

ifdef PLATFORM_WINDOWS
    # 外部関数の static 定義
    CFLAGS   += /DSUBFOLDER_SAMPLE_STATIC
    CXXFLAGS += /DSUBFOLDER_SAMPLE_STATIC
endif
