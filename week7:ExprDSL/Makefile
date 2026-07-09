CXXFLAGS=-std=c++17 -Wall -Wextra -pedantic

BIN=expr_demo
SRCS=main.cpp

all: $(BIN)


$(BIN): $(SRCS) expr.hpp
	g++ $(CXXFLAGS) $(SRCS) -o $(BIN)

clean:
	rm -f *~ *.o $(BIN)
