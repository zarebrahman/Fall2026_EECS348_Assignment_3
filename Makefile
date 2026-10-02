# EECS 348 Assignment 3 Makefile
# Builds the C++ program with g++.

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
TARGET = Assignment3
SOURCE = Assignment3.cpp

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) $(SOURCE) -o $(TARGET)

clean:
	rm -f $(TARGET)
