# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -O2 -I include

# Directories
SRC_DIR = src
TEST_DIR = test
DFA_DIR = data
EXAMPLES_DIR = examples
INCLUDE_DIR = include

# Binary
PASLEX = ./paslex

# Find all .cpp files recursively in src/
SRCS := $(shell find $(SRC_DIR) -name '*.cpp')

# make run rebuilds if source files changed
.PHONY: build run clean

build: 
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(PASLEX)

run: build
	$(PASLEX) $(TEST_DIR)/milestone-1/input/all_token.pas

clean:
	rm -f $(PASLEX)
