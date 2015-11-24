BIN=fileio.exe
LIB=libfio.a

MODULE_HEADER=FileIo.h Wave.h Csv.h
MODULE_SRC=Wave.cpp Csv.cpp

INC=$(MODULE_HEADER)

LIBS=-lwg

CPPFLAGS += -O3

WARNINGS +=  \
	-Werror \

include $(SELF_LEARNING_ROOT)/Build/Make/modulerules.mk
