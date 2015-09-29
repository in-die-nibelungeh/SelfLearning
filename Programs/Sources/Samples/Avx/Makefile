BIN=simd.exe

TOOLCHAIN_PREFIX := /c/MinGW/bin/mingw32-

MODULE_HEADER=
MODULE_SRC=

INC=$(MODULE_HEADER)

LIBS=

CPPFLAGS += -O3 -mavx

include $(MY_AUDIO_ROOT)/Build/Make/modulerules.mk

exec: $(BIN)
	@echo [AVX]
	@./$(BIN) 100000 100000 avx
	@echo [SSE]
	@./$(BIN) 100000 100000 sse
	@echo [NoOpt]
	@./$(BIN) 100000 100000

