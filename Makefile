CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -march=native
LDLIBS = -L/usr/local/lib -lsdsl -ldivsufsort -ldivsufsort64

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:src/%.cpp=build/%.o)

TARGET = build/multidocs


all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $^ $(LDLIBS) -o $@

build/%.o: src/%.cpp | build/
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/:
	mkdir -p build/

clean:
	rm -rf build/

.PHONY: all clean
