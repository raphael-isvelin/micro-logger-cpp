# Quick Makefile for micro-logger-cpp (header-only library)

CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2
INCLUDES = -I../inc

TARGET = micro_logger_sample
SOURCE = micro_logger_sample.cpp

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SOURCE) -o $(TARGET)

clean:
	rm -f $(TARGET) test.logs

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
