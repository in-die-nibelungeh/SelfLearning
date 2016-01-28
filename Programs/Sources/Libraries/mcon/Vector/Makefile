BIN=mcon_vector

MODULE_HEADER=$(SELF_LEARNING_INCDIR)/mcon/Vector.h
MODULE_SRC=

INC=$(MODULE_HEADER)
SRC= \
	../Vectord/Vectord.cpp \
	../Vectord/VectordBase.cpp \
    test_vector.cpp \
	test_matrix.cpp \
	benchmark_vector.cpp \

LIBS=-lmtbx -lmutl

CPPFLAGS += -O3 -mavx


WARNINGS +=  \
	-Werror \

include $(SELF_LEARNING_ROOT)/Build/Make/modulerules.mk
