#include<stdio.h>
#include<stdlib.h>
#include<string>
#include "ListaDeCoresRGB.h"

#include <GL/glut.h>

int drawText(std::string str, int x, int y, int cor) {
    defineCor(cor);
    glRasterPos3f(x, y, 0);
    for (int i = 0; i < str.length(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
    }
}

void init() 
{
    glClearColor(0.0f, 0.0f, 0.f, 1.0f);
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    // Definindo os limites lógicos da área OpenGL dentro da Janela
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glLineWidth(1);
    glColor3f(1, 1, 1);
    drawText("Test", -14, 11, Red);

    glutSwapBuffers(); // TODO: Saber o que significa isso de swapBuffers
}

int main(int argc, char**argv) {
    cout << "Programa OpenGL" << endl;
    
}