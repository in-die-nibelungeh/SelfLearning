BIN=mcon_matrix.exe

INC=$(SELF_LEARNING_INCDIR)/mcon/Matrix.h
SRC=

CPPFLAGS += \
    -O3 \
	-mavx \
	-DTestType=float \

WARNINGS +=  \
	-Werror \

include $(SELF_LEARNING_ROOT)/Build/Make/modulerules.mk
