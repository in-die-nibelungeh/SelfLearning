BIN=masp_ft

MODULE_HEADER=$(SELF_LEARNING_INCDIR)/masp/Ft.h
MODULE_SRC=Ft.cpp

INC=$(MODULE_HEADER)
SRC=test_Ft.cpp benchmark_Ft.cpp

LIBS=-lmtbx -lmutl -lmfio -lmcon

CPPFLAGS += -O3

include $(SELF_LEARNING_ROOT)/Build/Make/modulerules.mk

benchmark: $(BIN)
	./$(BIN) benchmark

.PHONY: benchmark
