
MODULE_NAME := mcon
LIB=libmcon.a

INC=$(addprefix $(SELF_LEARNING_INCDIR)/mcon/, Vector.h Matrix.h Vectord.h VectordBase.h Matrixd.h)

$(info $(INC))

MODULE_SRC=	\
	Vectord/VectordBase.cpp \
	Vectord/Vectord.cpp \
	Matrixd/Matrixd.cpp \

WARNINGS += -Werror
CPPFLAGS += -O3 -mavx

include $(SELF_LEARNING_ROOT)/Build/Make/modulerules.mk
