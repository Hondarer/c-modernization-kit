SUBDIRS = \
	prod \
	test

APP_NAME = $(notdir $(CURDIR))
DOXYFW_DIR = ../../framework/doxyfw
SUBDIR_TARGETS = $(addprefix __subdir__,$(SUBDIRS))

.DEFAULT_GOAL := default

.PHONY: default
default : SUBDIR_GOAL = default
default : $(SUBDIR_TARGETS)

.PHONY: clean
clean : SUBDIR_GOAL = clean
clean : $(SUBDIR_TARGETS)

.PHONY: test
test :
	@if [ -f test/makefile ]; then \
		echo $(MAKE) -C test test; \
		$(MAKE) -C test test || exit 1; \
	else \
		echo "Skipping directory 'test' (no makefile)"; \
	fi

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

.PHONY: $(SUBDIR_TARGETS)
$(SUBDIR_TARGETS) :
	@dir=$(patsubst __subdir__%,%,$@); \
	if [ -f $$dir/makefile ]; then \
		echo $(MAKE) -C $$dir $(SUBDIR_GOAL); \
		$(MAKE) -C $$dir $(SUBDIR_GOAL) || exit 1; \
	else \
		echo "Skipping directory '$$dir' (no makefile)"; \
	fi
