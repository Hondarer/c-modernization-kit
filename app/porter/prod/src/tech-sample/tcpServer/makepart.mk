# ライブラリの指定
LIBS += util
ifdef PLATFORM_WINDOWS
    # Windows では Winsock ライブラリをリンクする
    LIBS += ws2_32
endif
