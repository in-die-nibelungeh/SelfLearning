
MODULE_NAME := mcon
LIB=libcon.a

MODULE_HEADER= \
	Vectord/Vectord.h \
	Matrixdd/Matrixdd.h \

MODULE_SRC=	\
	Vectord/Vectord.cpp \
	Matrixd/Matrixd.cpp \

INC=$(MODULE_HEADER)

WARNINGS += -Werror
CPPFLAGS += -O3

include $(MY_AUDIO_ROOT)/Build/Make/modulerules.mk
