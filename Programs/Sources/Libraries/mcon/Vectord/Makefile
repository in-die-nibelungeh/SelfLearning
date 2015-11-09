BIN=mcon_vectord

MODULE_HEADER=VectordBase.h Vectord.h
MODULE_SRC=VectordBase.cpp Vectord.cpp

INC=$(MODULE_HEADER) tune.cpp
SRC=test_VectordBase.cpp test_Vectord.cpp benchmark_Vectord.cpp
INCDIRS += . ../Vector

LIBS=-lutl

CPPFLAGS += \
	-O3 \
	-mavx \

WARNINGS +=  \
	-Werror \

include $(MY_AUDIO_ROOT)/Build/Make/modulerules.mk
