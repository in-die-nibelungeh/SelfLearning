BIN=mcon_vector

MODULE_HEADER=Vector.h
MODULE_SRC=

INC=$(MODULE_HEADER) test_vector.cpp benchmark_vector.cpp

INCDIRS = . ../Matrix

LIBS=-ltbx -lutl

CPPFLAGS += -O3


WARNINGS +=  \
	-Werror \

include $(MY_AUDIO_ROOT)/Build/Make/modulerules.mk
