BIN=fft.exe
LIB=libfft.a

MODULE_HEADER=Fft.h
MODULE_SRC=Fft.cpp

INC=$(MODULE_HEADER) test.cpp tune.cpp

LIBS=-lwg -lbut -lfio

CPPFLAGS += -O3

include $(MY_AUDIO_ROOT)/Build/Make/modulerules.mk
