BIN=mcon_vectord

MODULE_HEADER=$(addprefix, $(SELF_LEARNING_INCDIR)/mcon/, VectordBase.h Vectord.h)
MODULE_SRC=VectordBase.cpp Vectord.cpp

SRC=test_VectordBase.cpp test_Vectord.cpp benchmark_Vectord.cpp tune.cpp

LIBS=-lmutl

CPPFLAGS += \
	-O3 \
	-mavx \

WARNINGS +=  \
	-Werror \

include $(SELF_LEARNING_ROOT)/Build/Make/modulerules.mk
