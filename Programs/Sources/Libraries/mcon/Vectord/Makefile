BIN=mcon_vectord.exe

MODULE_HEADER=Vectord.h
MODULE_SRC=

INC=$(MODULE_HEADER) test.cpp tune.cpp

LIBS=-lwg -lutl

CPPFLAGS += -O3

WARNINGS +=  \
	-Werror \

include $(MY_AUDIO_ROOT)/Build/Make/modulerules.mk
