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

#define ACTIVE_NPCS 10

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
  float speed;
  bool wasMadeDecision; // tentativa de acabar com o estado uito deterministico as trajetorias com decisoes sendo tomadas na msm curva
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
    float* lengths;
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
        p->currentCurveId = initialCurveId;
        p->nextCurveId = initialCurveId;
        p->t = 0.5f; // inicia no meio das curvas
        p->nextDirection = 1;
        //p->direction = 1;
        p->direction = initialDirection;
        for (int i = 0; i < maxBranches; i++) {
            p->possibleCurves[i].possibleCurveId = 0;
            p->possibleCurves[i].possibleDirection = 0;
        }
        p->totalPossibleCurves = 0;
        p->speed = 1.0f;
        p->wasMadeDecision = false;
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
                // TODO: velocidade baseada no comprimeto da curva
                c->visualCurves[i]->calculaComprimentoDaCurva();
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

void findNearbyCurves(int npcIndex) {
    int destiny; // o indice final ou indice inicial dependendo da direção da curva atual do npc

    NPC* currentNPC = world->npcs[npcIndex];
    currentNPC->totalPossibleCurves = 0; // limpando o os numeros aleatorios pq ele estava tomando decisoes malucas
    // pergunto a direcao antes de definir o destino
    if (currentNPC->direction == 1) {
        destiny = curvesIndices[currentNPC->currentCurveId].endIndex; 
    } else {
        destiny = curvesIndices[currentNPC->currentCurveId].startIndex; 
    }
    cout << "O NPC vai terminar a curva atual no ponto ID: " << destiny << endl;

    for(int i = 0; i < totalCurves; i++) {
        if (i == currentNPC->currentCurveId) {
            continue;
        }

        if (curvesIndices[i].startIndex == destiny) {
            currentNPC->possibleCurves[currentNPC->totalPossibleCurves].possibleCurveId = i;
            currentNPC->possibleCurves[currentNPC->totalPossibleCurves].possibleDirection = 1;
            currentNPC->totalPossibleCurves++;
            // npc->nextCurveId = i;
            //npc->nextDirection = 1;
            //break;
        } else if (curvesIndices[i].endIndex == destiny) {
            currentNPC->possibleCurves[currentNPC->totalPossibleCurves].possibleCurveId = i;
            currentNPC->possibleCurves[currentNPC->totalPossibleCurves].possibleDirection = -1;
            currentNPC->totalPossibleCurves++;
            // npc->nextCurveId = i;
            // npc->nextDirection = -1;
            //break;
        }
    }
}

void animateNPC(int value) {
    int rndNewCurveIndex;
    float timeFrame = 0.033f;
    for(int i = 0; i < ACTIVE_NPCS; i++) {
        NPC* currentNPC = world->npcs[i];
        float delta = currentNPC->speed * timeFrame;
        float deltaT = bezierCurves->visualCurves[currentNPC->currentCurveId]->CalculaT(delta);

        if (currentNPC->direction == 1) {
            // currentNPC->t += 0.005f;
            currentNPC->t += deltaT;
        } else {
            // currentNPC->t -= 0.005f;
            currentNPC->t -= deltaT;
        }

        if ((currentNPC->direction == 1 && currentNPC->t >= 1.0f) || 
        (currentNPC->direction == -1 && currentNPC->t <= 0.0f)) { 

            currentNPC->currentCurveId = currentNPC->nextCurveId;
            currentNPC->direction = currentNPC->nextDirection;
            //cout << "Entrando na nova curva=" << npc->currentCurveId << " | Direcao=" << npc->direction << endl;

            if (currentNPC->direction == 1) {
                currentNPC->t = 0.0f;
            } else {
                currentNPC->t = 1.0f;
            }
            currentNPC->wasMadeDecision = false;
        }

        bool wasCrossedMiddle = (currentNPC->direction == 1 && currentNPC->t >= 0.5f) || 
                             (currentNPC->direction == -1 && currentNPC->t <= 0.5f);

        //if (currentNPC->t >= 0.500f && currentNPC->t < 0.505f) {
        if (wasCrossedMiddle && !currentNPC->wasMadeDecision) {
            findNearbyCurves(i);
            if (currentNPC->totalPossibleCurves > 0) {
                rndNewCurveIndex = rand() % currentNPC->totalPossibleCurves;
                currentNPC->nextCurveId = currentNPC->possibleCurves[rndNewCurveIndex].possibleCurveId;
                currentNPC->nextDirection = currentNPC->possibleCurves[rndNewCurveIndex].possibleDirection;
            }
            //cout << "Decidindo nova curva= " << npc->nextCurveId << endl;
            currentNPC->wasMadeDecision = true;
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

float moveObject(int npcIndex) {
    float dx, dy, angleRads, angleDegrees;
    NPC* currentNPC = world->npcs[npcIndex];
    int curveId = currentNPC->currentCurveId;
    
    cout << "current curve=" << currentNPC->currentCurveId << endl;
    cout << "next curve=" << currentNPC->nextCurveId << endl; 
    /*
    aqui eu uso derivada (pela calcula ja da classe) pra descobrir para onde eu faço o triangulo deve virar
    seu vertice maior. Uso t + 0.001 pra saber onde o veiculo vai estar em 1 seg no futuro.
    */
    currentNPC->currentPosition = bezierCurves->visualCurves[curveId]->Calcula(currentNPC->t);
    if (currentNPC->direction == 1) {
        currentNPC->nextPosition = bezierCurves->visualCurves[curveId]->Calcula(currentNPC->t + 0.1);
    } else {
        currentNPC->nextPosition = bezierCurves->visualCurves[curveId]->Calcula(currentNPC->t - 0.1);
    }
    /*
    delta x e delta y sao calculados aqui. 
    se eu diminuo o ponto atual do proximo eu descubro o quanto eu andei no eixo X e eixo U.
    é basicamente desenhar um triangulo retangulo onde dx e dy sao catetos (base e altura, respec)
    e a hipotenusa é o vetor de deslocamento apontando exatamente pra direção do movimento.
    */
    dx = currentNPC->nextPosition.x - currentNPC->currentPosition.x;
    dy = currentNPC->nextPosition.y - currentNPC->currentPosition.y;

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
    
    for(int i = 0; i < ACTIVE_NPCS; i++) {
        angleDegrees = moveObject(i);
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

bool init() {
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

    world = (World*)malloc(sizeof(World));
    if (world == NULL) { return false; }
/*
curva escolhida aleatoriamente e sentido aleatorio tb
metade comeca em -1 e a outra metade 1d
*/
    for(int i = 0; i < ACTIVE_NPCS; i++) {
        int rndCurve = rand() % totalCurves;
        int rndDir = (i % 2 == 0) ? 1 : -1;
        
        world->npcs[i] = initNPC(rndCurve, rndDir);
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