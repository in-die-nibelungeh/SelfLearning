
MODULE_NAME := mcon
LIB=libmcon.a

MODULE_HEADER= \
	Vector/Vector.h \
	Matrix/Matrix.h \
	Vectord/Vectord.h \
	Vectord/VectordBase.h \
	Matrixd/Matrixd.h \

MODULE_SRC=	\
	Vectord/VectordBase.cpp \
	Vectord/Vectord.cpp \
	Matrixd/Matrixd.cpp \

INC=$(MODULE_HEADER)

INCDIRS += \
    Vector \
	Matrix \
    Vectord \
	Matrixd \

WARNINGS += -Werror
CPPFLAGS += -O3 -mavx

include $(MY_AUDIO_ROOT)/Build/Make/modulerules.mk
