SRC = \
	main.cpp \
	Fft.cpp \
	../WaveGen/sources/WaveGen.cpp \

INC = 
LIB = -lm -lstdc++
CPPFLAGS += -I../WaveGen/sources

BIN = fft.exe

all: build

run: $(BIN)
	@$(BIN)

build: $(SRC) $(INC)
	gcc $(CPPFLAGS) -o $(BIN) $(SRC) $(LIB)

clean:
	@if [ -f "$(BIN)" ] ; then rm $(BIN) ; fi

.PHONY: clean run all build 
