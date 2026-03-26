# Windows では Winsock ライブラリをリンクする
ifeq ($(OS),Windows_NT)
    LIBS += ws2_32
endif
