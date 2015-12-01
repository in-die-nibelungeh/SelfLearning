
MODULE_NAME := mutl
LIB=libmutl.a

MODULE_HEADER= \
	$(SELF_LEARNING_INCDIR)/mutl/Stopwatch.h \
	$(SELF_LEARNING_INCDIR)/mutl/Clockwatch.h \
	$(SELF_LEARNING_INCDIR)/mutl/Linear.h \
	$(SELF_LEARNING_INCDIR)/mutl/Spline.h \

MODULE_SRC=	\
	Benchmark/Stopwatch.cpp \
	Benchmark/Clockwatch.cpp \
	Interpolator/Linear.cpp \
	Interpolator/Spline.cpp \

INC=$(MODULE_HEADER)

WARNINGS += -Werror
CPPFLAGS += -O3 \

include $(SELF_LEARNING_ROOT)/Build/Make/modulerules.mk
