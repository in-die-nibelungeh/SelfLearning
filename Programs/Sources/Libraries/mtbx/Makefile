
MODULE_NAME := mtbx
LIB=libmtbx.a

MODULE_HEADER= \
	WaveGen/WaveGen.h \

MODULE_SRC=	\
	WaveGen/WaveGen.cpp \

INC=$(MODULE_HEADER)

WARNINGS += -Werror
CPPFLAGS += -O3

include $(MY_AUDIO_ROOT)/Build/Make/modulerules.mk
