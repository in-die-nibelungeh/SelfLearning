BIN=mcon_vectord.exe

MODULE_HEADER=VectordBase.h Vectord.h
MODULE_SRC=VectordBase.cpp Vectord.cpp

INC=$(MODULE_HEADER) test.cpp tune.cpp
INCDIRS += ../Vector

LIBS=-lutl

CPPFLAGS += \
	-O3 \
	-mavx \

WARNINGS +=  \
	-Werror \

include $(MY_AUDIO_ROOT)/Build/Make/modulerules.mk
