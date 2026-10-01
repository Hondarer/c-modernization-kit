# ライブラリの指定
LIBS += cplat cjson struct_meta

# struct-meta-gen (ヘッダー解析ツール) の実行体。
# prod/src/cmd/makelocal.mk の SUBDIRS 宣言順により、必ずこの makepart.mk の
# 評価より前に struct-meta-gen のビルドが完了している。
ifdef PLATFORM_LINUX
STRUCT_META_GEN_BIN := $(MYAPP_DIR)/prod/cbin/struct-meta-gen
STRUCT_META_GEN_RUN := LD_LIBRARY_PATH="$(MYAPP_DIR)/prod/cbin$${LD_LIBRARY_PATH:+:$${LD_LIBRARY_PATH}}" "$(STRUCT_META_GEN_BIN)"
else ifdef PLATFORM_WINDOWS
STRUCT_META_GEN_BIN := $(MYAPP_DIR)/prod/cbin/struct-meta-gen.exe
STRUCT_META_GEN_RUN := "$(STRUCT_META_GEN_BIN)"
endif

# 解析対象ヘッダーを静的に宣言する。ヘッダー内の typedef struct をすべて変換する。
# 生成される .c / .h のパスはこの宣言からパース時点で決定論的に導出できるため、
# 実行時発見 (pre-build フックやワイルドカード) は不要。
STRUCT_META_GEN_HEADERS := sample_types.h

# GENDIR は _flags.mk (このファイルより後に読み込まれる) で定義されるため、
# ここでは変数を参照できない。GENDIR は常にワークスペース共通の "gen" なので、
# リテラルを直接使う (framework/makefw/docs/makeparts.md の例 6 を参照)。
_struct_meta_gen_gendir := gen

_struct_meta_gen_stem = $(basename $(1))

# ヘッダーごとに .c を生成する。struct-meta-gen は同名の .h も副作用として書き出す。
# 複数ターゲットを 1 行に並べると GNU Make が規則を複製して 2 回起動するため、
# 正本は .c だけにし、.h は .c に依存させる。
# gen/*.c -> obj/*.o のコンパイルは framework 側の _flex_bison_compile.mk が
# GENDIR_EXTRA_C 経由で汎用的に扱う。
define _STRUCT_META_GEN_RULE
$(_struct_meta_gen_gendir)/$(call _struct_meta_gen_stem,$(1))_meta.c: $(1) $(call _makefw_escape_path,$(STRUCT_META_GEN_BIN)) | $(_struct_meta_gen_gendir)
	@echo "struct-meta-gen --header $(1)"
	$(STRUCT_META_GEN_RUN) --header "$(1)" --out "$$@"
$(_struct_meta_gen_gendir)/$(call _struct_meta_gen_stem,$(1))_meta.h: $(_struct_meta_gen_gendir)/$(call _struct_meta_gen_stem,$(1))_meta.c
	@test -f "$$@"
endef
$(foreach h,$(STRUCT_META_GEN_HEADERS),$(eval $(call _STRUCT_META_GEN_RULE,$(h))))

GENDIR_EXTRA_C += $(foreach h,$(STRUCT_META_GEN_HEADERS),$(_struct_meta_gen_gendir)/$(call _struct_meta_gen_stem,$(h))_meta.c)

# struct_meta_sample.c が生成ヘッダーを #include するため、初回ビルドでも
# ヘッダー生成が先行するよう明示依存を置く。OBJDIR は makepart.mk 評価時点では
# 未定義なので、GENDIR と同様にリテラル obj を使う。
obj/struct_meta_sample.o obj/struct_meta_sample.obj: \
	$(foreach h,$(STRUCT_META_GEN_HEADERS),$(_struct_meta_gen_gendir)/$(call _struct_meta_gen_stem,$(h))_meta.h)
