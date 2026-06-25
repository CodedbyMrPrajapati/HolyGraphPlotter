CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall
LIBS = -lglew32 -lglfw3 -lopengl32 -lgdi32

TARGET = rubik.exe
SRCS = main.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

clean:
	del /Q $(TARGET) 2>NUL || rm -f $(TARGET)
