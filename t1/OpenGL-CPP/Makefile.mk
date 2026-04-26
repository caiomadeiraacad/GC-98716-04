# Makefile para Windows

PROG = ProgOpenGL.exe
#FONTES = Linha.cpp Ponto.cpp Temporizador.cpp InterseccaoEntreTodasAsLinhas.cpp 
#FONTES = Ponto.cpp Poligono.cpp Temporizador.cpp ListaDeCoresRGB.cpp ProgramaBasicoOpenGL.cpp
#FONTES = ExemploDeManipulacaoDeImagens.cpp ImageClass.cpp Temporizador.cpp SOIL/SOIL.cpp SOIL/image_DXT.cpp SOIL/image_helper.cpp SOIL/stb_image_aug.cpp
FONTES = Ponto.cpp Poligono.cpp Temporizador.cpp ListaDeCoresRGB.cpp ProgramaBasicoOpenGL1.cpp
#FONTES = Ponto.cpp Poligono.cpp Temporizador.cpp ListaDeCoresRGB.cpp Bezier.cpp DesenhaBezier.cpp

OBJETOS = $(FONTES:.cpp=.cpp)

CFLAGS = -g -Iinclude\GL  -Wno-write-strings -Wno-narrowing # -Wall -g  # Todas as warnings, infos de debug
#LDFLAGS = -Llib\GL -lfreeglut -lopengl32 -lglu32 -lm
LDFLAGS = -Llib -lfreeglut -lopengl32 -lglu32 -lm
CC = g++

$(PROG): $(OBJETOS)
	g++ $(CFLAGS) $(OBJETOS) -o $@ $(LDFLAGS)

clean:
	rm $(OBJETOS) $(PROG)
