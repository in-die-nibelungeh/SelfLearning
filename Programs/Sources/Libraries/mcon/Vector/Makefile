BIN=mcon_vector

MODULE_HEADER=Vector.h
MODULE_SRC=

INC=$(MODULE_HEADER) test_vector.cpp benchmark_vector.cpp
SRC=

INCDIRS = . ../Matrix ../Vectord

LIBS=-ltbx -lutl

CPPFLAGS += -O3


WARNINGS +=  \
	-Werror \

include $(SELF_LEARNING_ROOT)/Build/Make/modulerules.mk
