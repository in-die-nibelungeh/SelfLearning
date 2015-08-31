
BIN ?= a.exe

LIBS += -lstdc++ -lm

PROJECT_INCDIR := $(MY_AUDIO_ROOT)/Programs/Include
PROJECT_LIBDIR := $(MY_AUDIO_ROOT)/Programs/Library

INCDIRS += $(PROJECT_INCDIR)
LIBDIRS += $(PROJECT_LIBDIR)

ENTRY_SRC ?= main.cpp

MODULE_OBJ := \
	$(MODULE_SRC:.cpp=.o) \
#	$(MODULE_SRC:.c=.o)   \
#	$(MODULE_SRC:.s=.o)   \
#	$(MODULE_SRC:.S=.o)

INC += $(addprefix $(PROJECT_INCDIR)/, types.h status.h debug.h) $(MODULE_HEADER)
OBJ := $(MODULE_OBJ) $(ENTRY_SRC:*.cpp=.o)

CPPFLAGS += \
	$(addprefix -I,$(INCDIRS)) \
	$(addprefix -L,$(LIBDIRS)) \

all: $(BIN)
	@echo Succeeded in building $(BIN).

$(BIN): $(OBJ) $(INC)
	gcc $(CPPFLAGS) -o $(BIN) $(OBJ) $(LIBS)

lib: $(MODULE_OBJ)
	@if [ -n "$(LIB)" ] ; then \
		ar r $(LIB) $(MODULE_OBJ) ; \
	else \
		echo LIB is not define. ; \
	fi

install: $(LIB) $(MODULE_HEADER)
	@if [ -n "$(LIB)" ] ; then \
		cp $(LIB) $(PROJECT_LIBDIR)/ ; \
	fi
	cp $(MODULE_HEADER) $(PROJECT_INCDIR)/

clean: 
	@rm -rf $(BIN) $(LIB) *.BAK *.o

.cpp.o: $(INC)
	gcc -c $(CPPFLAGS) $<

.PHONY: all lib install clean build
