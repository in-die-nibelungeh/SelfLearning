
BIN=masp_win
LIBS=-lmtbx -lmfio -lmcon

MODULE_HEADER=$(SELF_LEARNING_INCDIR)/masp/Window.h
MODULE_SRC=Window.cpp

INC=$(MODULE_HEADER)
SRC= \
    ../Fir/Fir.cpp \
    ../Ft/Ft.cpp \

CPPFLAGS += -O3

include $(SELF_LEARNING_ROOT)/Build/Make/modulerules.mk
