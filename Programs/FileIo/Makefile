SRC = \
	main.cpp \
	FileIo.cpp

INC = 
LIB = -lm -lstdc++ -lwg
CPPFLAGS += \
	-I$(MY_AUDIO_ROOT)/Programs/Include \
	-L$(MY_AUDIO_ROOT)/Programs/Library \

BIN = fileio.exe

all: build

run: $(BIN)
	@$(BIN)

build: $(SRC) $(INC)
	gcc $(CPPFLAGS) -o $(BIN) $(SRC) $(LIB)

clean:
	@if [ -f "$(BIN)" ] ; then rm $(BIN) ; fi

.PHONY: clean run all build 
