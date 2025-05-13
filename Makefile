CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2

# targets
TARGET = subnx 
TEST_TARGET = test

# headers
HEADERS = subnx.h utils.h cmdline.h
TEST_HEADERS = subnx.h utils.h

# sources
SRCS = main.cpp subnx.cpp utils.cpp
TEST_SRCS = test.cpp subnx.cpp utils.cpp

all: $(TARGET)

debug: CXXFLAGS += -g -O0 -DDEBUG
debug: $(TARGET)

$(TARGET): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

$(TEST_TARGET): $(TEST_SRCS) $(TEST_HEADERS)
	$(CXX) $(CXXFLAGS) $(TEST_SRCS) -o $(TEST_TARGET)

clean:
	rm -f $(TARGET) $(TEST_TARGET)

.PHONY: all debug clean test
