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
COMPILER = ./compiler

# Find all .cpp files recursively in src/
SRCS := $(shell find $(SRC_DIR) -name '*.cpp' ! -name 'test_*.cpp')
HEADERS := $(shell find $(INCLUDE_DIR) -name '*.hpp')

# make run rebuilds if source files changed
.PHONY: build run clean

build: 
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(COMPILER)

run:
	$(COMPILER) $(TEST_DIR)/milestone-3/input/simple_hello.pas --decorated

all:clean build run
clean:
	rm -f $(COMPILER)
