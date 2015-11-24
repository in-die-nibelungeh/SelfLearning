
MODULE_NAME := mutl
LIB=libmutl.a

MODULE_HEADER= \
	Benchmark/Stopwatch.h \
	Benchmark/Clockwatch.h \
	Interpolator/Linear.h \
	Interpolator/Spline.h \

MODULE_SRC=	\
	Benchmark/Stopwatch.cpp \
	Benchmark/Clockwatch.cpp \
	Interpolator/Linear.cpp \
	Interpolator/Spline.cpp \

INC=$(MODULE_HEADER)

WARNINGS += -Werror
CPPFLAGS += -O3 \

include $(SELF_LEARNING_ROOT)/Build/Make/modulerules.mk
