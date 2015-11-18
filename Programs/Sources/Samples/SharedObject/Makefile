BIN=so
SO=so.dll

MODULE_HEADER=
MODULE_SRC=

_SRC=so.cpp

INC=$(MODULE_HEADER) $(SO)

LIBS=-lso

CPPFLAGS += -O3 -L./

include $(MY_AUDIO_ROOT)/Build/Make/modulerules.mk

$(SO):	$(_SRC:.cpp=.o)
	$(CC) -shared -o $(SO) $<

