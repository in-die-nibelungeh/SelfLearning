
MODULE_NAME := masp
LIB=libasp.a

MODULE_HEADER= \
	Basics/Fir/Fir.h \
	Basics/Fft/Fft.h \
	Basics/Iir/Iir.h \
	Basics/Window/Window.h \
	Resampler/Resampler.h \

MODULE_SRC=	\
	Basics/Fir/Fir.cpp \
	Basics/Iir/Iir.cpp \
	Basics/Fft/Fft.cpp \
	Basics/Window/Window.cpp \
	Resampler/Resampler.cpp \

INC=$(MODULE_HEADER)

CPPFLAGS += -O3

include $(MY_AUDIO_ROOT)/Build/Make/modulerules.mk
