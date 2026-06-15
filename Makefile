CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Icpp_core/include
SRC = $(wildcard cpp_core/src/*.cpp)
BIN_DIR = build
BIN = $(BIN_DIR)/charity_app

ifeq ($(OS),Windows_NT)
    BIN = $(BIN_DIR)/charity_app.exe
endif

all: $(BIN)

$(BIN): $(SRC)
	mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(BIN)

run: $(BIN)
	./$(BIN)

seed: $(BIN)
	./$(BIN) seed

test: $(BIN)
	python3 tests/run_core_tests.py

clean:
	rm -rf $(BIN_DIR)

.PHONY: all run seed test clean
