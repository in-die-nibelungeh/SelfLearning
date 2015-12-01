BIN=mcon_matrixd.exe

MODULE_HEADER=$(addprefix $(SELF_LEARNING_INCDIR)/mcon/,Matrixd.h Vectord.h VectordBase.h)
MODULE_SRC=Matrixd.cpp

SRC=test_Matrixd.cpp benchmark_Matrixd.cpp ../Vectord/Vectord.cpp ../Vectord/VectordBase.cpp

LIBS=-lmutl

CPPFLAGS += \
	-O3 \
	-mavx \

WARNINGS +=  \
	-Werror \

include $(SELF_LEARNING_ROOT)/Build/Make/modulerules.mk
