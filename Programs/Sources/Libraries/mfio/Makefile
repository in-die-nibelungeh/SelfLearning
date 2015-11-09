
MODULE_NAME := mfio
LIB=libmfio.a

MODULE_HEADER= \
	Csv/Csv.h \
	Wave/Wave.h \

MODULE_SRC=	\
	Csv/Csv.cpp \
	Wave/Wave.cpp \

INC=$(MODULE_HEADER)

CPPFLAGS += -O3

include $(MY_AUDIO_ROOT)/Build/Make/modulerules.mk
