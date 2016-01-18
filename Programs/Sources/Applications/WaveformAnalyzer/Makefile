BIN=wanalyzer

INC = Common.h

SRC = Setup.cpp \
    PreProcess.cpp \
    Process.cpp \

LIBS = -lmasp -lmfio

CPPFLAGS += -O3 \
	-DPROGRAM_NAME="\"$(BIN)\"" \

include $(SELF_LEARNING_ROOT)/Build/Make/modulerules.mk
