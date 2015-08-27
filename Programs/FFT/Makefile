
ENTRY_SRC = main.cpp
MODULE_SRC = Fft.cpp

SRC = $(ENTRY_SRC) $(MODULE_SRC)
OBJ = $(MODULE_SRC:.cpp=.o) $(ENTRY_SRC:.cpp=.o)

MODULE_OBJ = $(MODULE_SRC:.cpp=.o)

INC = 
LIBS = -lwg
CPPFLAGS += \
	-I$(MY_AUDIO_ROOT)/Programs/Include \
	-L$(MY_AUDIO_ROOT)/Programs/Library \

LIB = libfft.a
MODULE_HEADER = Fft.h

BIN = fft.exe

all: build

run: $(BIN)
	@$(BIN)

.cpp.o: $(INC)
	gcc -c $(CPPFLAGS) $<

build: $(OBJ) $(INC)
	gcc $(CPPFLAGS) -o $(BIN) $(SRC) $(LIBS)

lib: $(MODULE_OBJ)
	ar r $(LIB) $(MODULE_OBJ)

install: $(LIB) $(MODULE_HEADER)
	cp $(LIB) $(MY_AUDIO_ROOT)/Programs/Library/
	cp $(MODULE_HEADER) $(MY_AUDIO_ROOT)/Programs/Include/

clean:
	@if [ -f "$(BIN)" ] ; then rm $(BIN) ; fi

.PHONY: clean run all build 
