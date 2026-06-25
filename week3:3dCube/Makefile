OPENGLLIB= -lGL
GLEWLIB= -lGLEW
GLFWLIB = -lglfw
LIBS=$(OPENGLLIB) $(GLEWLIB) $(GLFWLIB)
LDFLAGS=-L/usr/local/lib
CPPFLAGS=-I/usr/local/include
CXXFLAGS=-std=c++17 -Wall -Wextra

BIN=cube
SRCS=main.cpp ../gl_framework.cpp ../shader_util.cpp
INCLUDES=

all: $(BIN)


$(BIN): $(SRCS)
	g++ $(CXXFLAGS) $(CPPFLAGS) $(SRCS) -o $(BIN) $(LDFLAGS) $(LIBS)

clean:
	rm -f *~ *.o $(BIN)