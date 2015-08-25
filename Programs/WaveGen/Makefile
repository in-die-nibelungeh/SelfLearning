
BIN=wavgen.exe

LIB=libwg.a
HEADER=WaveGen.h

SRC=WaveGen.cpp main.cpp
OBJ=$(SRC:.cpp=.o)
INC=$(MY_AUDIO_ROOT)/Programs/Include/types.h $(HEADER)

$(info OBJ=$(OBJ))

CPPFLAGS += -I$(MY_AUDIO_ROOT)/Programs/Include

all: $(BIN)
	@echo Succeeded in building $(BIN).

$(BIN): $(SRC) $(INC)
	gcc $(CPPFLAGS) -o $(BIN) $(SRC) -lm -lstdc++

lib: $(OBJ)
	ar r $(LIB) $(OBJ)

.cpp.o: $(INC)
	gcc -c $(CPPFLAGS) $<

install: $(LIB) $(HEADER)
	cp $(LIB) $(MY_AUDIO_ROOT)/Programs/Library/
	cp $(HEADER) $(MY_AUDIO_ROOT)/Programs/Include/

clean: 
	@rm -rf $(BIN) $(LIB) *.BAK 

