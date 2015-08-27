
clean:
	@echo Deleting *.BAK, *.o, *.exe ...
	@find ./ -name '*.BAK' -o -name '*.o' -o -name '*.exe' -exec rm {} \;
	@echo Deleting *.a ...
	@find ./ -path ./Programs/Library -prune -o -name '*.a' -print -exec rm {} \;
	@echo Cleaned up.

.PHONY: clean

