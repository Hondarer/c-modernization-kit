# 利用側の定義: この app が直接利用する app を指定する。
APP_DEPS := cplat cjson

# struct-meta-gen (prod でビルドする生成器) が struct-meta-sample の gen/ のソースを作る。
# make gen は生成器を必要とするため、この app と依存先の prod をビルドする。
APP_GEN_REQUIRES_BUILD := 1
