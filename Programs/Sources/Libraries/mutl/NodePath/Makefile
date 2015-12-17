
BIN=node

MODULE_HEADER=
MODULE_SRC=

INC=$(MODULE_HEADER) $(SELF_LEARNING_INCDIR)/mutl/NodePath.h

CPPFLAGS += -O3

include $(SELF_LEARNING_ROOT)/Build/Make/modulerules.mk

test: $(BIN)
	./$(BIN) -o sine a -p -v b -a c d e -be tan -bf ab3 4 5 6 7

.PHONY: test
