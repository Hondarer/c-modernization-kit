LIB_TYPE = static

# ライブラリの指定
LIBS += cplat
ifdef PLATFORM_WINDOWS
    LIBS += ws2_32
endif
