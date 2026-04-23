#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<iostream>
#include <math.h>

#include "GL/glut.h"

#include "OpenGL-CPP/Poligono.h"
#include "OpenGL-CPP/Ponto.h"
#include "OpenGL-CPP/Bezier.h"
#include "OpenGL-CPP/ListaDeCoresRGB.h"

using namespace std;

typedef struct {
    int p0, p1, p2;
} CurvesPoints;

int totalCurves;
float t = 0.0f;
Poligono controlPoints; // controlPoints
Poligono polygon;
CurvesPoints* curves;
Bezier visualCurve;

void SetTrackPolygon(FILE* fp, int numControlPoints, Poligono* polygon);

int loadFile(const char* filename, Poligono* polygon, CurvesPoints* curves, int maxCurves) {
    FILE * fp = fopen(filename, "r");
    if (!fp) {
        printf("Error\n");
        exit(-1);
    }

    int numControlPoints;
    if (fscanf(fp, "%d", &numControlPoints) != 1) {
        fclose(fp);
        return 0;
    }

    SetTrackPolygon(fp, numControlPoints, polygon);

    int numCurves;
    fscanf(fp, "%d", &numCurves);

    if (numCurves > maxCurves) {
        numCurves = maxCurves;
    }

    for(int i = 0; i < numCurves; i++) {
        fscanf(fp, "%d %d %d", &curves[i].p0, &curves[i].p1, &curves[i].p2);
    }

    fclose(fp);
    return numCurves;
}

void SetTrackPolygon(FILE* fp, int numControlPoints, Poligono* polygon) {
    for(int i = 0; i < numControlPoints; i++) {
        float x, y;
        fscanf(fp, "%f %f", &x, &y);
        Ponto pt;
        pt.set(x, y, 0.0f);
        polygon->insereVertice(pt);
    }
}

void animateVehicle(int value) {
    t += 0.005f;

    if (t > 1.0f) {
        t = 0.0f;
    }

    glutPostRedisplay();
    glutTimerFunc(33, animateVehicle, 1);
}

void DrawVehicle(void) 
{
    glRotatef(2.0f, 2.0f, 2.0f, 1.0f);
    glScalef(0.05f, 0.05f, 0.0f);
    glBegin(GL_TRIANGLES);
        glVertex2f(-5.0f, -5.0f);
        glVertex2f( 5.0f, -5.0f);
        glVertex2f( 0.0f,  10.0f);
    glEnd();
    glPopMatrix();
}

void display(void) {
    
    glClear(GL_COLOR_BUFFER_BIT);

    Ponto min, max;
    controlPoints.obtemLimites(min, max);
    Ponto currentPosition = visualCurve.Calcula(t);
    Ponto nextPosition = visualCurve.Calcula(t + 0.001);
    float dx = nextPosition.x - currentPosition.x;
    float dy = nextPosition.y - currentPosition.y;
    float angleRads = atan2(dy, dx);
    float angleDegrees = angleRads * 180.0 / M_PI;

    glPushMatrix();
        glTranslatef(currentPosition.x, nextPosition.y, 0.0f);
         glRotatef(angleDegrees - 90.0f, 0.0f, 0.0f, 1.0f);
         DrawVehicle();
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(min.x - 0.5, max.x + 0.5, min.y - 0.5, max.y + 0.5);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    defineCor(GreenYellow);
    Ponto p0, p1, p2;
    p0 = Ponto(-10, -10);
    p1 = Ponto(0, 10);
    p2 = Ponto(10, -10);

    glColor3f(1, 1, 1);

    for(int i = 0; i < totalCurves; i++) {
        Ponto p0, p1, p2;
        p0 = controlPoints.getVertice(curves[i].p0);
        p1 = controlPoints.getVertice(curves[i].p1);
        p2 = controlPoints.getVertice(curves[i].p2);
        //glPushMatrix();
            visualCurve = Bezier(p0, p1, p2);
            visualCurve.Traca();
            visualCurve.cor = 2;
        //glPopMatrix();
    }

    glutSwapBuffers();
}

int main(int argc, char** argv) {

    controlPoints = Poligono();

    curves = (CurvesPoints*)malloc(50*sizeof(CurvesPoints));
    if (curves == NULL) { 
        printf("erro curvespoints is NULL"); 
        exit(-1);
    }

    cout << "T1 - Caio Madeira" << endl;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
    glutInitWindowPosition(0, 0);

    glutInitWindowSize(800, 800);

    glutCreateWindow("T1 - Caio Madeira");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    totalCurves = loadFile("polygon.txt", &controlPoints, curves, 50);
    if (totalCurves == 0) {
        cout << "numCurves = 0. Exiting...";
        exit(-1);
    }

    //controlPoints.atualizaLimites();

    glutDisplayFunc(display);
    glutTimerFunc(33, animateVehicle, 1);
    glutMainLoop();

    return 0;
}