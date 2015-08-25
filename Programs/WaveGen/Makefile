
BIN=wavgen.exe
SRC=WaveGen.cpp main.cpp
INC=types.h

all: $(BIN)
	@echo Succeeded in building $(BIN).

$(BIN): $(SRC) $(INC)
	gcc -I../../Common/include -o $(BIN) WaveGen.cpp main.cpp -lm -lstdc++

clean: $(BIN)
	@rm -rf $(BIN) *.BAK

