BIN=mcon_matrixd.exe

MODULE_HEADER=Matrixd.h
MODULE_SRC=Matrixd.cpp

INC=$(MODULE_HEADER)
SRC=test_Matrixd.cpp ../Vectord/Vectord.cpp ../Vectord/VectordBase.cpp

INCDIRS += \
	../Vector  \
	../Vectord \

LIBS=-lutl

CPPFLAGS += \
	-O3 \
	-mavx \

WARNINGS +=  \
	-Werror \

include $(MY_AUDIO_ROOT)/Build/Make/modulerules.mk
