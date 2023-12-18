# Compiler
CC=g++
TARGET=btree
OBJ_DIR := obj
SRC_FILES = $(wildcard *.cpp)
OBJ_FILES = $(SRC_FILES:%.cpp=$(OBJ_DIR)/%.o)
CFLAGS=

ifdef DEBUG
	CFLAGS += -g
endif

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) $^ -o $(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
