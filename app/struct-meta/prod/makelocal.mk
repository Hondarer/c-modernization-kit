# BEGIN makefw-subdirs
SUBDIRS := \
	libsrc \
	src
# END makefw-subdirs

include $(call _makefw_escape_path,$(APP_DIR)/cplat/prod/runtime-bundle.mk)

# struct-meta-gen は解析とレイアウト計算を libstruct_meta へ委ねるため、自 app の
# 実行時ライブラリも生成器の隣へ置く。Linux の LD_LIBRARY_PATH と Windows の
# DLL 探索のどちらでも、生成器と同じディレクトリにあれば見つかる。
STRUCT_META_RUNTIME_OUTPUT_DIR := $(MYAPP_DIR)/prod/cbin

ifdef PLATFORM_LINUX
    STRUCT_META_RUNTIME_LIBRARY := libstruct_meta.so
else ifdef PLATFORM_WINDOWS
    STRUCT_META_RUNTIME_LIBRARY := libstruct_meta.dll
endif

STRUCT_META_RUNTIME_SOURCE := $(MYAPP_DIR)/prod/lib/$(STRUCT_META_RUNTIME_LIBRARY)

.PHONY: struct-meta-runtime-bundle struct-meta-runtime-clean

struct-meta-runtime-bundle:
	mkdir -p "$(STRUCT_META_RUNTIME_OUTPUT_DIR)"
	cp -f "$(STRUCT_META_RUNTIME_SOURCE)" "$(STRUCT_META_RUNTIME_OUTPUT_DIR)/$(STRUCT_META_RUNTIME_LIBRARY)"

struct-meta-runtime-clean:
	rm -f "$(STRUCT_META_RUNTIME_OUTPUT_DIR)/$(STRUCT_META_RUNTIME_LIBRARY)"

# struct-meta-gen は src のビルド中に実行するため、実行時ライブラリを先に配置する。
# libstruct_meta は libsrc が作るため、その配置は libsrc の完了後に行う。
# clean は SUBDIRS 経由で src を辿るため、この依存を clean 時に付けると
# 依存 app の成果物削除後にコピーして失敗する。
ifeq ($(filter clean,$(MAKECMDGOALS)),)
src: cplat-runtime-bundle struct-meta-runtime-bundle
struct-meta-runtime-bundle: libsrc
endif

default build: cplat-runtime-bundle

clean: cplat-runtime-clean struct-meta-runtime-clean
