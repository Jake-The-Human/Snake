OBJS = snake.o game.o window.o file.o main.o
CC = clang++ -std=c++17 -Wall
LINKER_FLAGS = -lSDL2 -lSDL2_ttf -lSDL2_mixer
OBJ_NAME = snake
OBJ_DIR = ./bin/

all: $(OBJS)
	$(CC) $(OBJS) $(LINKER_FLAGS) -o $(OBJ_NAME)

snake_test: snake.o snake_test.o
	$(CC) snake.o snake_test.o -o snake_test

snake.o: snake.cpp
	$(CC) -c snake.cpp

game.o: game.cpp
	$(CC) -c game.cpp

window.o: window.cpp
	$(CC) -c window.cpp

file.o: file.cpp
	$(CC) -c file.cpp

main.o: main.cpp
	$(CC) -c main.cpp

clean:
	rm *.o snake snake_test
