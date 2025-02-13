# Compiler
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -Iinc

# Directories
SRC_DIR := src
INC_DIR := inc
OBJ_DIR := obj
BIN_DIR := bin

# Output executable
TARGET := $(BIN_DIR)/main

# Find all .cpp files and generate corresponding .o files
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Default rule
all: $(TARGET)

# Create output binary
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compile .cpp files into .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create necessary directories
$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Run the program
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
