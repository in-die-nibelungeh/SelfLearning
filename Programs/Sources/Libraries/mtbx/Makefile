
MODULE_NAME := mtbx
LIB=libmtbx.a

MODULE_HEADER= \
	$(SELF_LEARNING_INCDIR)/mtbx/WaveGen.h \

MODULE_SRC=	\
	WaveGen/WaveGen.cpp \

INC=$(MODULE_HEADER)

WARNINGS += -Werror
CPPFLAGS += -O3

include $(SELF_LEARNING_ROOT)/Build/Make/modulerules.mk
