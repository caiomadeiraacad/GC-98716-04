CC = g++
INC_DIR = ./include
CFLAGS = -Wall -I$(INC_DIR)

LIBS = -lglut -lGLU -lGL
TARGET = T1 
 
SRCS = main.cpp OpenGL-CPP/Ponto.cpp OpenGL-CPP/Poligono.cpp OpenGL-CPP/Bezier.cpp OpenGL-CPP/ListaDeCoresRGB.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LIBS)


%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean