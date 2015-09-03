
GOALS_EXCLUDED := clean

SUBDIRS := Programs \

$(filter-out $(GOALS_EXCLUDED),$(MAKECMDGOALS)) _root: $(SUBDIRS)
	$(MAKE) -C Programs $(MAKEGOALS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

clean:
	@echo Deleting *.BAK, *.o, *.exe, *.stackdump ...
	@find ./ -name '*.BAK' -exec rm {} \; -o -name '*.o' -exec rm {} \; -o -name '*.exe' -exec rm {} \; -o -name '*.stackdump' -exec rm {} \;
	@echo Deleting *.a ...
	@find ./ -path ./Programs/Library -prune -o -name '*.a' -print -exec rm {} \;
	@echo Cleaned up.

.PHONY: _root $(GOALS_EXCLUDED) $(SUBDIRS)

