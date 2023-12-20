CC := g++
TARGET := btree
OBJ_DIR := obj
SRC_DIR := src
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(SRC_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
CFLAGS :=

ifdef DEBUG
    CFLAGS += -g
endif

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) $^ -o $(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)