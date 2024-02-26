# Compiler
CC := g++
# Compiler flags
CFLAGS := -Wall -Wextra -std=c++11
# Include directories
INC_DIR := include
# Source directories
SRC_DIR := src
# Object directories
OBJ_DIR := obj
# Binary directories
BIN_DIR := bin

# Source files
SRC := $(wildcard $(SRC_DIR)/*.cpp)
# Object files
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Output executable
TARGET := $(BIN_DIR)/test_controller

# Library name
LIB_NAME := aiplayercontroller

# Library file
LIB := $(BIN_DIR)/lib$(LIB_NAME).a

# Compiler flags for building the library
LIB_FLAGS := -c

# Include directories for compilation
INC := -I$(INC_DIR)

# Main target
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJ) $(LIB)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(INC) $(OBJ) -o $@ -L$(BIN_DIR) -l$(LIB_NAME)

# Rule to build the object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INC) $(LIB_FLAGS) $< -o $@

# Rule to build the library
$(LIB): $(OBJ)
	@mkdir -p $(BIN_DIR)
	ar rcs $@ $^

# Phony target to clean the project
clean:
	$(RM) -r $(OBJ_DIR) $(BIN_DIR)

# Phony target to clean and build the project
rebuild: clean all

# Declare the phony targets
.PHONY: all clean rebuild
