# Makefile para Linux
PROG = ProgOpenGL3D
FONTES = Ponto.cpp Poligono.cpp Temporizador.cpp ListaDeCoresRGB.cpp ProgramaBasico3D.cpp
OBJETOS = $(FONTES:.cpp=.o)

CFLAGS = -std=c++14 -g -Wall

LDFLAGS = -lGL -lGLU -lglut -lm
LIBS = -lglut -lGLU -lGL

CC = g++

$(PROG): $(OBJETOS)
	$(CC) $(CFLAGS) $(OBJETOS) -o $@ $(LDFLAGS) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-@ rm -f $(OBJETOS) $(PROG)

.PHONY: all clean