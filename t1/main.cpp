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

typedef enum {
    PLAYER,
    ENEMY
} EntityType;

typedef struct {
    int possibleCurveId;
    int possibleDirection;
} PossibleCurves;

typedef struct {
  int id;
  Ponto currentPosition;
  Ponto nextPosition;
  int nextCurveId;
  int currentCurveId;
  int nextDirection; // direcao p npc saber onde comecar (-1 ou 1)
  int direction; // direction = 1 -> sentido é o ponto inicial da curva; direction = -1 -> sentido é o ponto final da curva.
  PossibleCurves possibleCurves[20];
  int totalPossibleCurves;
  float t; // basicamente o progresso dele na curva. o deslocamento
} NPC;

typedef struct {
    NPC* npcs[11];
} World;

/*
20/04: coloquei esse nome pois guarda o indice de cada ponto da curva.
Pra apenas uma curva. O conjunto completo forma a topologia = como tudo esta conectado e
o nome Curves antes estava confuso.
*/
typedef struct {
    int startIndex, controlIndex, endIndex;
} CurvesIndices;


typedef struct {
    Bezier** visualCurves;
} BezierCurves;


int totalCurves;
// float t = 0.0f;
Poligono controlPoints; // controlPoints
Poligono polygon;
CurvesIndices* curvesIndices;
BezierCurves* bezierCurves; // guarda as curvas que vao se desenhadas
//NPC* npc;
World* world;

NPC* initNPC(int initialCurveId, int initialDirection) {
    int maxBranches = 20;
    NPC* p = new NPC();
    if (p != NULL) {
        //p->currentPosition = Ponto(0.0f, 0.0f);
        p->id = 0;
        p->currentPosition = Ponto(0.0f, 0.0f);
        p->nextPosition = Ponto(0.0f, 0.0f);
        p->currentCurveId = 0;
        p->nextCurveId = 0;
        p->t = 0.0f;
        p->nextDirection = 1;
        //p->direction = 1;
        p->direction = initialDirection;
        for (int i = 0; i < maxBranches; i++) {
            p->possibleCurves[i].possibleCurveId = 0;
            p->possibleCurves[i].possibleDirection = 0;
        }
        p->totalPossibleCurves = 0;
        return p;
    }
    return NULL;
}

BezierCurves* initCurves() {
    Ponto p0, p1, p2;

    BezierCurves* c = (BezierCurves*)malloc(sizeof(BezierCurves));
    if (c != NULL) {
        c->visualCurves = (Bezier**)malloc(totalCurves*sizeof(Bezier*));
        if (c->visualCurves != NULL) {
            for(int i = 0; i < totalCurves; i++) {
                p0 = controlPoints.getVertice(curvesIndices[i].startIndex);
                p1 = controlPoints.getVertice(curvesIndices[i].controlIndex);
                p2 = controlPoints.getVertice(curvesIndices[i].endIndex);
                c->visualCurves[i] = new Bezier(p0, p1, p2, 5);
            }
            return c;
        }
    }
    return NULL;
}

int loadFile(const char* filename, Poligono* polygon, CurvesIndices* curvesIndices, int maxCurves) {
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

    for(int i = 0; i < numControlPoints; i++) {
        float x, y;
        fscanf(fp, "%f %f", &x, &y);
        Ponto pt;
        pt.set(x, y, 0.0f);
        polygon->insereVertice(pt);
    }

    int numCurves;
    fscanf(fp, "%d", &numCurves);

    if (numCurves > maxCurves) {
        numCurves = maxCurves;
    }

    for(int i = 0; i < numCurves; i++) {
        fscanf(fp, "%d %d %d", 
            &curvesIndices[i].startIndex, 
            &curvesIndices[i].controlIndex, 
            &curvesIndices[i].endIndex);
    }

    fclose(fp);
    return numCurves;
}

void findNearbyCurves(void) {
    int destiny; // o indice final ou indice inicial dependendo da direção da curva atual do npc

    for(int i = 0; i < totalCurves - 3; i++) {
        // npc->totalPossibleCurves = 0; // limpando o os numeros aleatorios pq ele estava tomando decisoes malucas
        //world->npcs[i]->possibleCurves = 0;
        // pergunto a direcao antes de definir o destino
        if (world->npcs[i]->direction == 1) {
            destiny = curvesIndices[world->npcs[i]->currentCurveId].endIndex; 
        } else {
            destiny = curvesIndices[world->npcs[i]->currentCurveId].startIndex; 
        }
        cout << "O NPC vai terminar a curva atual no ponto ID: " << destiny << endl;

        for(int i = 0; i < totalCurves; i++) {
            if (i == world->npcs[i]->currentCurveId) {
                continue;
            }

            if (curvesIndices[i].startIndex == destiny) {
                world->npcs[i]->possibleCurves[world->npcs[i]->totalPossibleCurves].possibleCurveId = i;
                world->npcs[i]->possibleCurves[world->npcs[i]->totalPossibleCurves].possibleDirection = 1;
                world->npcs[i]->totalPossibleCurves++;
                // npc->nextCurveId = i;
                //npc->nextDirection = 1;
                //break;
            } else if (curvesIndices[i].endIndex == destiny) {
                world->npcs[i]->possibleCurves[world->npcs[i]->totalPossibleCurves].possibleCurveId = i;
                world->npcs[i]->possibleCurves[world->npcs[i]->totalPossibleCurves].possibleDirection = -1;
                world->npcs[i]->totalPossibleCurves++;
                // npc->nextCurveId = i;
                // npc->nextDirection = -1;
                //break;
            }
        }
    }
}

void animateNPC(int value) {
    int rndNewCurveIndex;
    for(int i = 0; i < totalCurves - 3; i++) {
        if (world->npcs[i]->direction == 1) {
            world->npcs[i]->t += 0.005f;
        } else {
            world->npcs[i]->t -= 0.005f;
        }

        if ((world->npcs[i]->direction == 1 && world->npcs[i]->t >= 1.0f) || (world->npcs[i]->direction == -1 && world->npcs[i]->t <= 0.0f)) { 
            world->npcs[i]->currentCurveId = world->npcs[i]->nextCurveId;
            world->npcs[i]->direction = world->npcs[i]->nextDirection;
            //cout << "Entrando na nova curva=" << npc->currentCurveId << " | Direcao=" << npc->direction << endl;

            if (world->npcs[i]->direction == 1) {
                world->npcs[i]->t = 0.0f;
            } else {
                world->npcs[i]->t = 1.0f;
            }
        }

        if (world->npcs[i]->t >= 0.500f && world->npcs[i]->t < 0.505f) {
            findNearbyCurves();
            if (world->npcs[i]->totalPossibleCurves > 0) {
                rndNewCurveIndex = rand() % world->npcs[i]->totalPossibleCurves;
                world->npcs[i]->nextCurveId = world->npcs[i]->possibleCurves[rndNewCurveIndex].possibleCurveId;
                world->npcs[i]->nextDirection = world->npcs[i]->possibleCurves[rndNewCurveIndex].possibleDirection;
            }
            //cout << "Decidindo nova curva= " << npc->nextCurveId << endl;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(33, animateNPC, 1);
}

void DrawVehicle(void)
{
    glRotatef(2.0f, 2.0f, 2.0f, 1.0f);
    glScalef(0.1f, 0.1f, 0.1f);
    defineCor(OrangeRed);
    glBegin(GL_TRIANGLES);
        glVertex2f(0.0f, 1.0f);
        glVertex2f(-1.0f, -1.0f);
        glVertex2f(1.0f,  -1.0f);
    glEnd();
}

float moveObject(int& i) {
    float dx, dy, angleRads, angleDegrees;
    cout << "current curve=" << world->npcs[i]->currentCurveId << endl;
    cout << "next curve=" << world->npcs[i]->nextCurveId << endl; 
    /*
    aqui eu uso derivada (pela calcula ja da classe) pra descobrir para onde eu faço o triangulo deve virar
    seu vertice maior. Uso t + 0.001 pra saber onde o veiculo vai estar em 1 seg no futuro.
    */
    world->npcs[i]->currentPosition = bezierCurves->visualCurves[i]->Calcula(world->npcs[i]->t);
    if (world->npcs[i]->direction == 1) {
        world->npcs[i]->nextPosition = bezierCurves->visualCurves[i]->Calcula(world->npcs[i]->t + 0.1);
    } else {
        world->npcs[i]->nextPosition = bezierCurves->visualCurves[i]->Calcula(world->npcs[i]->t - 0.1);
    }
    /*
    delta x e delta y sao calculados aqui. 
    se eu diminuo o ponto atual do proximo eu descubro o quanto eu andei no eixo X e eixo U.
    é basicamente desenhar um triangulo retangulo onde dx e dy sao catetos (base e altura, respec)
    e a hipotenusa é o vetor de deslocamento apontando exatamente pra direção do movimento.
    */
    dx = world->npcs[i]->nextPosition.x - world->npcs[i]->currentPosition.x;
    dy = world->npcs[i]->nextPosition.y - world->npcs[i]->currentPosition.y;

    /*
    tan = cat opt (dy)/ cat adj (dx). O arcotangente me da o angulo.
    a atan normal pode dar problemas com divisao por 0.
    depois, converto pra graus pro opengl pois atan2 retorna em radianos.
    graus = rads * (180/PI)
    */
   angleRads = atan2(dy, dx);
   angleDegrees = angleRads * 180.0 / M_PI;
   return angleDegrees;
}

void display(void) {
    Ponto max;
    Ponto min;
    float angleDegrees;
    int i;
    glClear(GL_COLOR_BUFFER_BIT);
    // varre tds os vertices do arquivo de texto e descobre o min x, min y, max x e max y e passa pro gluOrtho2D
    controlPoints.obtemLimites(min, max);
    for(i = 0; i < totalCurves; i++) {
        defineCor(i % 15);
        bezierCurves->visualCurves[i]->Traca();
    }
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(min.x - 0.5, max.x + 0.5, min.y - 0.5, max.y + 0.5);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    for(int i = 0; totalCurves - 3; i++) {
        angleDegrees = moveObject(world->npcs[i]->currentCurveId);
        glPushMatrix();
            glTranslatef(world->npcs[i]->currentPosition.x, world->npcs[i]->currentPosition.y, 0.0f);
            glRotatef(angleDegrees - 90.0f, 0.0f, 0.0f, 1.0f);
            DrawVehicle();
        glPopMatrix();
    }
    glutSwapBuffers();
}

// fiz pra impedir o resize da janela principal pra nao distorcer os componentes graficos
void reshape(int w, int h) {
    if (w != 800 || h!= 800) {
        glutReshapeWindow(800, 800);
    }
}

/*
TODO: Se eu quiser que eles spawnem em curvas aleatorias, eu preciso definir o range de pontos que aquela curva
pode comportar. EX: curva de ID = 12 -> [Ponto(0, 0), Ponto(2, 4)].
*/

void spawnEntity(EntityType entityType) {
    switch(entityType) {
        case PLAYER:
            break;
        case ENEMY:
            for(int i = 0; i < totalCurves - 3; i++) {
                int randCurveId = rand() % i;
                world->npcs[i] = initNPC(i, 1);
                //npc = initNPC(0, 1);
                //npc = initNPC(1, 1);
            }
            break;
        default:
            break;
    }
}

bool init() {
    // NPC* npc1;
    // NPC* npc2;
    // npc1 = initNPC(0, 1);
    // npc2 = initNPC(1, -1);
    // npc = initNPC(0, 1);

    World* world = (World*)malloc(sizeof(World));
    if (world == NULL) { printf(">> ERROR: ALLOC WORLD\n"); exit(-1); }
    for(int i = 0; i < totalCurves - 3; i++) {
        world->npcs[i] = NULL;
        //world->npcs[i] = initNPC(i, 1);

    }

    //spawnEntity(ENEMY);
    controlPoints = Poligono();

    curvesIndices = (CurvesIndices*)malloc(50*sizeof(CurvesIndices));
    if (curvesIndices == NULL) { 
        printf("error Curves is NULL"); 
        return false;
    }

    totalCurves = loadFile("polygon.txt", &controlPoints, curvesIndices, 50);
    if (totalCurves == 0) {
        cout << "numCurves = 0. Exiting...";
        return false;
    }

    bezierCurves = initCurves();
    if (bezierCurves == NULL) { 
        printf("error BezierCurves is NULL"); 
        return false;
    }

    return true;
}

// TODO: implementar liberação das estruturas em memória.
void freeAll() {}

int main(int argc, char** argv) {

    srand(time(NULL));
    if (!init()) { exit(-1); }
    cout << "T1 - Caio Madeira" << endl;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
    glutInitWindowPosition(0, 0);

    glutInitWindowSize(800, 800);
    glutCreateWindow("T1 - Caio Madeira");
    glutReshapeFunc(reshape);
    glClearColor(250.0f, 250.0f, 250.0f, 1.0f);


    glutDisplayFunc(display);
    glutTimerFunc(33, animateNPC, 1);
    glutMainLoop();

    return 0;
}