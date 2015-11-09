
MODULE_NAME := masp
LIB=libmasp.a

MODULE_HEADER= \
	Basics/Fir/Fir.h \
	Basics/Ft/Ft.h \
	Basics/Iir/Iir.h \
	Basics/Window/Window.h \
	Resampler/Resampler.h \

MODULE_SRC=	\
	Basics/Fir/Fir.cpp \
	Basics/Iir/Iir.cpp \
	Basics/Ft/Ft.cpp \
	Basics/Window/Window.cpp \
	Resampler/Resampler.cpp \

INCDIRS = \
	Basics/Ft \
	Basics/Fir \
	Basics/Iir \
	Basics/Window \
	Resampler \

INC=$(MODULE_HEADER)

CPPFLAGS += -O3

include $(MY_AUDIO_ROOT)/Build/Make/modulerules.mk
