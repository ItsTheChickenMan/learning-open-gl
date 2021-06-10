NAME=practice
CC=g++

INC_DIR=./include/
LIB_DIR=./lib/
BIN_DIR=./bin/
SRC_DIR=./src/

_SRC=*.cpp glad/glad.c
SRC=$(patsubst %,$(SRC_DIR)%,$(_SRC))

LIBS=-lglfw3 -lopengl32 -lgdi32 -luser32 -lkernel32

CFLAGS=-I$(INC_DIR) -L$(LIB_DIR) $(LIBS) -Wall -Wno-write-strings

all:
	$(CC) $(SRC) -o $(BIN_DIR)$(NAME) $(CFLAGS) $(LIBS)