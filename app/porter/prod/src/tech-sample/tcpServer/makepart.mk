# ライブラリの指定
LIBS += com_util
ifdef PLATFORM_WINDOWS
    # Windows では Winsock ライブラリをリンクする
    LIBS += ws2_32
endif
