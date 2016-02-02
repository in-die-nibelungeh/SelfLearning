
BIN=masp_fir
LIBS=-lmfio -lmcon

MODULE_HEADER=$(SELF_LEARNING_INCDIR)/masp/Fir.h

INC=$(MODULE_HEADER)
SRC= \
    Fir.cpp \
    ../Ft/Ft.cpp \
    ../Window/Window.cpp \

CPPFLAGS += -O3

include $(SELF_LEARNING_ROOT)/Build/Make/modulerules.mk
