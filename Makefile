build:
	g++ -o chip8_emulator main.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

all: build