
SUBDIRS_EXCLUDE := Makefile Include Library Samples
SUBDIRS := $(filter-out $(SUBDIRS_EXCLUDE), $(wildcard *))

$(MAKECMDGOALS) _root: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: _root $(SUBDIRS)
