
SUBDIRS_EXCLUDE := 
SUBDIRS := $(filter-out $(SUBDIRS_EXCLUDE), $(wildcard *))

$(MAKECMDGOALS) _root: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: _root $(SUBDIRS)
