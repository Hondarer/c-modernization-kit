# ライブラリの指定
LIBS += process_orchestration cplat
ifdef PLATFORM_WINDOWS
    LIBS += ws2_32
endif
