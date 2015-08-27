
BIN=wavgen.exe

LIB=libwg.a
HEADER=WaveGen.h

ENTRY_SRC=main.cpp
MODULE_SRC=WaveGen.cpp
SRC = $(ENTRY_SRC) $(MODULE_SRC)
OBJ=$(MODULE_SRC:.cpp=.o)
INC=$(MY_AUDIO_ROOT)/Programs/Include/types.h $(HEADER)

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
	@rm -rf $(BIN) $(LIB) *.BAK *.o

