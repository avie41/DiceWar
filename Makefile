PROJECTS := GenMap StrategyDummy

ifdef debug
	DIR = linux/debug
	SUFFIX = _d
else
	DIR = linux/release
	SUFFIX =
endif

.PHONY: all $(PROJECTS)
all: $(PROJECTS)

export debug

$(PROJECTS):
	@echo "-------------------------------------------"
	@echo "--- Compilation of the project '$@'"
	@echo "---"
	@$(MAKE) -C $@

run:
	bin/$(DIR)/dicewars$(SUFFIX) -r bin/$(DIR)/referee$(SUFFIX).so -m bin/$(DIR)/genmap$(SUFFIX).so -g bin/$(DIR)/gui$(SUFFIX).so -s bin/$(DIR)/strategy$(SUFFIX).so -s bin/$(DIR)/strategy$(SUFFIX).so
