
BIN ?= 
NOECHO ?= @
OUTPUT_DIR ?= .
OBJECT_DIR ?= .

ifndef MY_AUDIO_ROOT
$(error Define MY_AUDIO_ROOT)
endif

ifdef LIB
ifndef MODULE_NAME
$(error Define MODULE_NAME)
endif
endif

ifdef BIN
MODULE_BIN_PATH := $(OUTPUT_DIR)/$(BIN)
else
MODULE_BIN_PATH :=
endif

ifdef LIB
MODULE_LIB_PATH := $(OUTPUT_DIR)/$(LIB)
else
MODULE_LIB_PATH :=
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

all: build
	@echo Succeeded in building.

run: $(MODULE_BIN_PATH)
	@if [ -n "$(<)" ] ; then \
		echo Running $(<) ... ; $(<) \
	else \
		echo Nothing to be done for \`$(@)\': BIN is not defined. ; \
	fi

build: $(MODULE_BIN_PATH) $(MODULE_LIB_PATH)

lib: $(MODULE_LIB_PATH)

$(MODULE_BIN_PATH): $(OBJ) $(INC)
	$(NOECHO)$(call Mkdir,$(dir $@))
	$(CC) $(CPPFLAGS) -o $@ $(OBJ) $(LIBS)

$(MODULE_LIB_PATH): $(MODULE_OBJ)
	@if [ -n "$(LIB)" ] ; then \
		ar r $@ $(MODULE_OBJ) ; \
	else \
		echo LIB is not define. ; \
	fi

install: $(MODULE_LIB_PATH) $(MODULE_HEADER)
	@if [ -n "$(LIB)" ] ; then \
		echo cp $(MODULE_LIB_PATH) $(PROJECT_LIBDIR)/ ; \
		cp $(MODULE_LIB_PATH) $(PROJECT_LIBDIR)/ ; \
	fi
	@if [ -n "$(MODULE_HEADER)" ] ; then \
		$(call Mkdir,$(MODULE_HEADER_INSTALL_DIR)) ; \
		cp $(MODULE_HEADER) $(MODULE_HEADER_INSTALL_DIR)/ ; \
	fi
	@if [ -z "$(LIB)" -a -z "$(MODULE_HEADER)" ] ; then \
		echo Nothing to be done for \`$(@)\': Neither LIB nor MODULE_HEADER is defined. ; \
	fi

clean: 
	@rm -rf $(MODULE_BIN_PATH) $(MODULE_LIB_PATH) *.BAK $(MODULE_OBJ)

%.o: %.cpp $(INC)
	$(CC) -c $(CPPFLAGS) $< -o $@

.PHONY: all lib install clean build run
