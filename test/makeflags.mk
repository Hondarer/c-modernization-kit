ifneq ($(OS),Windows_NT)
    # Linux
CFLAGS=\
	-Wall \
	-Wextra \
	-Wcast-align \
	-Wdisabled-optimization \
	-Wfloat-equal \
	-Wformat=2 \
	-Winit-self \
	-Winline \
	-Winvalid-pch \
	-Wmissing-format-attribute \
	-Wmissing-noreturn \
	-Wpacked \
	-Wpadded \
	-Wredundant-decls \
	-Wstrict-aliasing=2 \
	-Wswitch-default \
	-Wswitch-enum \
	-Wundef \
	-Wvariadic-macros \
	-Wshadow \
	-Wmissing-declarations \
	-Wmissing-prototypes \
	-Wstrict-prototypes \
	-Wsequence-point \
	-Wpointer-arith \
	-Wbad-function-cast \
	-Wunsafe-loop-optimizations \
	-Wnested-externs \
	-Wcast-qual \
	-Wint-to-pointer-cast \
	-Wpointer-to-int-cast \
	-Wimplicit-fallthrough=0 \
	-Wformat-overflow=0 \
	-Wunknown-pragmas \
	-W
CXXFLAGS=\
	-Wall \
	-Wextra \
	-Wcast-align \
	-Wdisabled-optimization \
	-Wfloat-equal \
	-Wformat=2 \
	-Winit-self \
	-Winline \
	-Winvalid-pch \
	-Wmissing-format-attribute \
	-Wmissing-noreturn \
	-Wpacked \
	-Wpadded \
	-Wredundant-decls \
	-Wstrict-aliasing=2 \
	-Wswitch-default \
	-Wswitch-enum \
	-Wundef \
	-Wvariadic-macros \
	-Wshadow \
	-Wmissing-declarations \
	-Wsequence-point \
	-Wpointer-arith \
	-Wunsafe-loop-optimizations \
	-Wcast-qual \
	-Wint-to-pointer-cast \
	-Wimplicit-fallthrough=0 \
	-Wformat-overflow=0 \
	-Wunknown-pragmas \
	-W
LDFLAGS     =
else
    # Windows
CFLAGS      =
CXXFLAGS    =
LDFLAGS     =
endif

LIBSDIR    +=\
	$(WORKSPACE_FOLDER)/testfw/lib \
	$(WORKSPACE_FOLDER)/test/lib
