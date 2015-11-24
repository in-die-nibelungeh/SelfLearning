BIN=mcon_matrixd.exe

MODULE_HEADER=Matrixd.h
MODULE_SRC=Matrixd.cpp

INC=$(MODULE_HEADER) ../Vectord/Vectord.h ../Vectord/VectordBase.h
SRC=test_Matrixd.cpp benchmark_Matrixd.cpp ../Vectord/Vectord.cpp ../Vectord/VectordBase.cpp

INCDIRS += \
	../Vector  \
	../Matrix  \
	../Vectord \

LIBS=-lutl

CPPFLAGS += \
	-O3 \
	-mavx \

WARNINGS +=  \
	-Werror \

include $(SELF_LEARNING_ROOT)/Build/Make/modulerules.mk
