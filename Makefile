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
HEADERS := $(shell find $(INCLUDE_DIR) -name '*.hpp')

# make run rebuilds if source files changed
.PHONY: build run clean

build: 
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(PASLEX)

run:
	$(PASLEX) $(TEST_DIR)/milestone-2/input/testwhile.pas

all:clean build run
clean:
	rm -f $(PASLEX)
