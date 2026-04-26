# Makefile para Linux e macOS

PROG = ProgOpenGL
#FONTES = Linha.cpp Ponto.cpp InterseccaoEntreTodasAsLinhas.cpp Temporizador.cpp
#FONTES = Ponto.cpp Poligono.cpp Temporizador.cpp ListaDeCoresRGB.cpp ProgramaBasicoOpenGL.cpp 
#FONTES = ExemploDeManipulacaoDeImagens.cpp ImageClass.cpp Temporizador.cpp SOIL/SOIL.cpp SOIL/image_DXT.cpp SOIL/image_helper.cpp SOIL/stb_image_aug.cpp

FONTES = Ponto.cpp Poligono.cpp Temporizador.cpp ListaDeCoresRGB.cpp ProgramaBasicoOpenGL.cpp

OBJETOS = $(FONTES:.cpp=.o)
CPPFLAGS = -g -O3 -DGL_SILENCE_DEPRECATION -Wno-write-strings -Wno-narrowing -Wno-stringop-overflow # -Wall -g  # Todas as warnings, infos de debug

UNAME = `uname`

all: $(TARGET)
	-@make $(UNAME)

Darwin: $(OBJETOS)
#	g++ $(OBJETOS) -O3 -Wno-deprecated -framework OpenGL -framework Cocoa -framework GLUT -lm -o $(PROG)
	g++ $(OBJETOS) -O3 -framework OpenGL -framework Cocoa -framework GLUT -lm -o $(PROG)

Linux: $(OBJETOS)
	g++ $(OBJETOS) -O3 -lGL -lGLU -lglut -lm -o $(PROG)

clean:
	-@ rm -f $(OBJETOS) $(PROG)
