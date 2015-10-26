BIN=mcon_vectord.exe

MODULE_HEADER=Vectord.h
MODULE_SRC=Vectord.cpp

INC=$(MODULE_HEADER) test.cpp tune.cpp
INCDIRS += ../Vector

LIBS=-lwg -lutl

CPPFLAGS += \
	-O3 \
	#-std=c++11 \

WARNINGS +=  \
	-Werror \

include $(MY_AUDIO_ROOT)/Build/Make/modulerules.mk
