SUBDIRS = $(wildcard */)
APP_NAME = $(notdir $(CURDIR))
DOXYFW_DIR = ../../framework/doxyfw

.DEFAULT_GOAL := default

.PHONY: default
default : $(SUBDIRS)

.PHONY: clean
clean : $(SUBDIRS)

.PHONY: test
test : $(SUBDIRS)

.PHONY: doxy
doxy :
	@if [ -f Doxyfile.part ]; then \
		if [ -d $(DOXYFW_DIR) ] && [ -f $(DOXYFW_DIR)/makefile ]; then \
			echo $(MAKE) -C $(DOXYFW_DIR) CATEGORY=$(APP_NAME); \
			$(MAKE) -C $(DOXYFW_DIR) CATEGORY=$(APP_NAME) || exit 1; \
		else \
			echo "INFO: $(DOXYFW_DIR) directory not found, skipping."; \
		fi; \
	else \
		echo "INFO: Doxygen is not configured for $(APP_NAME), skipping."; \
	fi

.PHONY: $(SUBDIRS)
$(SUBDIRS) :
	@if [ -f $@/makefile ]; then \
		echo $(MAKE) -C $@ $(MAKECMDGOALS); \
		$(MAKE) -C $@ $(MAKECMDGOALS) || exit 1; \
	else \
		echo "Skipping directory '$@' (no makefile)"; \
	fi
