CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -march=native
LDLIBS = -L/usr/local/lib -lsdsl -ldivsufsort -ldivsufsort64

FM_SRC = src/fm_main.cpp src/FMIndex.cpp
FM_OBJ = $(FM_SRC:src/%.cpp=build/%.o)
FM = build/fm_exec

SA_SRC =
SA_OBJ =
SA = build/sa_exec

#all: $(FM) $(SA)
all: $(FM)

$(FM): $(FM_OBJ)
	$(CXX) $^ $(LDLIBS) -o $@

build/%.o: src/%.cpp | build/
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/:
	mkdir -p build/

clean:
	rm -rf build/

.PHONY: all clean
