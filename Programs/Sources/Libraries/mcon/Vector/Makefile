BIN=buffer.exe

MODULE_HEADER=Vector.h Matrix.h
MODULE_SRC=

INC=$(MODULE_HEADER) test_vector.cpp test_matrix.cpp benchmark_vector.cpp

LIBS=-lwg -lbut

CPPFLAGS += -O3

WARNINGS +=  \
	-Werror \

include $(MY_AUDIO_ROOT)/Build/Make/modulerules.mk
