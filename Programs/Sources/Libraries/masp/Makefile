
MODULE_NAME := masp
LIB=libmasp.a

MODULE_HEADER= \
	$(addprefix $(SELF_LEARNING_INCDIR)/masp/,Fir.h Iir.h Ft.h Window.h Resampler.h)

MODULE_SRC=	\
	Basics/Fir/Fir.cpp \
	Basics/Iir/Iir.cpp \
	Basics/Ft/Ft.cpp \
	Basics/Window/Window.cpp \
	Resampler/Resampler.cpp \

INC=$(MODULE_HEADER)

WARNINGS += -Werror
CPPFLAGS += -O3

include $(SELF_LEARNING_ROOT)/Build/Make/modulerules.mk
