help:
	@echo "INFO: make <TAB> for targets"
.PHONY: help

run:
	make -C sys sys
	make -C sdk sdk gui
.PHONY: run

clean:
	make -C sys clean
	make -C sdk clean
.PHONY: clean

ultraclean:
	make -C sys ultraclean
	make -C sdk ultraclean
.PHONY: ultraclean
