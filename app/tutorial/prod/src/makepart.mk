# ライブラリの指定
# 単体で配布・実行できるように、実行時ライブラリは prod/cbin へ同梱する。
# see: app/cplat/docs/link-policy.md
LIBS += cplat
ifdef PLATFORM_LINUX
    LDFLAGS += -Wl,-z,origin -Wl,-rpath,'$$ORIGIN'
endif
