BIN=mcon_matrixd.exe

MODULE_HEADER=Matrixd.h
MODULE_SRC=

INC=$(MODULE_HEADER) test.cpp

INCDIRS += \
	../Vectord \

LIBS=-lwg -lutl

CPPFLAGS += -O3

WARNINGS +=  \
	-Werror \

include $(MY_AUDIO_ROOT)/Build/Make/modulerules.mk
