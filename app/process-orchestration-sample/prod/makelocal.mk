# BEGIN makefw-subdirs
SUBDIRS := \
	libsrc \
	src
# END makefw-subdirs

include $(call _makefw_escape_path,$(APP_DIR)/cplat/prod/runtime-bundle.mk)

ifeq ($(filter clean,$(MAKECMDGOALS)),)
src: cplat-runtime-bundle
endif

default build: cplat-runtime-bundle

clean: cplat-runtime-clean
