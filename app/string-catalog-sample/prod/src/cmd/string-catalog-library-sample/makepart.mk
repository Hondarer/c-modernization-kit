# 依存ライブラリ
# samplecatalog が公開するカタログを利用する。
# トレース種別の関数形式マクロは、呼び出し側のコンパイル単位で cplat の関数を
# 展開するため、利用側も cplat をリンクする。
LIBS += samplecatalog cplat
