
BIN ?= 
NOECHO ?= 

ifndef MY_AUDIO_ROOT
$(error Define MY_AUDIO_ROOT)
endif

ifdef LIB
ifndef MODULE_NAME
$(error Define MODULE_NAME)
endif
endif

CC := $(TOOLCHAIN_PREFIX)gcc.exe
LD := $(TOOLCHAIN_PREFIX)ld.exe

LIBS += -lstdc++ -lm

PROJECT_INCDIR := $(MY_AUDIO_ROOT)/Programs/Include
PROJECT_LIBDIR := $(MY_AUDIO_ROOT)/Programs/Library

MODULE_HEADER_INSTALL_DIR := $(PROJECT_INCDIR)/$(MODULE_NAME)

INCDIRS += $(PROJECT_INCDIR)
LIBDIRS += $(PROJECT_LIBDIR)

ENTRY_SRC ?= main.cpp

MODULE_BUILD ?= Debug
OUTDIR ?= .

ifneq ($(MODULE_BUILD),Debug)
ifneq ($(MODULE_BUILD),Release)
$(error Invalid build target: $(MODULE_BUILD))
endif
endif

ifeq ($(MODULE_BUILD),Debug)
    CPPFLAGS += -DDEBUG -g
endif

MODULE_OBJ := \
	$(MODULE_SRC:.cpp=.o) \
#	$(MODULE_SRC:.c=.o)   \
#	$(MODULE_SRC:.s=.o)   \
#	$(MODULE_SRC:.S=.o)

INC += $(addprefix $(PROJECT_INCDIR)/, types.h status.h debug.h) $(MODULE_HEADER)
OBJ := $(MODULE_OBJ) $(ENTRY_SRC:*.cpp=.o)

WARNINGS += -Wall \


CPPFLAGS += \
	$(addprefix -I,$(INCDIRS)) \
	$(addprefix -L,$(LIBDIRS)) \
	$(WARNINGS) \

# Macros
# Mkdir=if not exist @1 mkdir @1
Mkdir=if [ ! -d "$(1)" ] ; then mkdir -p "$(1)" ; fi

all: $(BIN)
	@echo Succeeded in building $(BIN).

run: $(BIN)
	@echo Running $(BIN) ...
	@./$(BIN)

build: $(BIN) lib

lib: $(LIB)

$(BIN): $(OBJ) $(INC)
	$(CC) $(CPPFLAGS) -o $(BIN) $(OBJ) $(LIBS)

$(LIB): $(MODULE_OBJ)
	@if [ -n "$(LIB)" ] ; then \
		ar r $(LIB) $(MODULE_OBJ) ; \
	else \
		echo LIB is not define. ; \
	fi

install: $(LIB) $(MODULE_HEADER)
	@if [ -n "$(LIB)" ] ; then \
		echo cp $(LIB) $(PROJECT_LIBDIR)/ ; \
		cp $(LIB) $(PROJECT_LIBDIR)/ ; \
	fi
	$(NOECHO)$(call Mkdir, $(MODULE_HEADER_INSTALL_DIR))
	cp $(MODULE_HEADER) $(MODULE_HEADER_INSTALL_DIR)/

clean: 
	@rm -rf $(BIN) $(LIB) *.BAK *.o

.cpp.o: $(INC)
	$(CC) -c $(CPPFLAGS) $< -o $(<:.cpp=.o)

.PHONY: all lib install clean build run
