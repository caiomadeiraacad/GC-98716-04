    // **********************************************************************
// PUCRS/Escola Polit�cnica
// COMPUTA��O GR�FICA
//
// Programa basico para criar aplicacoes 2D em OpenGL
//
// Marcio Sarroglia Pinho
// pinho@pucrs.br
// **********************************************************************
  
// Para uso no Xcode:
// Abra o menu Product -> Scheme -> Edit Scheme -> Use custom working directory
// Selecione a pasta onde voce descompactou o ZIP que continha este arquivo.
//
//  Para rodar em um terminal Windows, digite
//           mingw32-make -f Makefile.mk 
//

#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>


using namespace std;

#ifdef WIN32
#include <windows.h>
#include <glut.h>
#else
#include <sys/time.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <glut.h>
#endif

#include "Ponto.h"
#include "Poligono.h"
#include "Bezier.h"

#include "Temporizador.h"
#include "ListaDeCoresRGB.h"

Temporizador T;
double AccumDeltaT=0;
Temporizador T2;

Bezier Curvas[20];
unsigned int nCurvas;
Ponto PontosClicados[3];
int nPontoAtual=0;

Poligono PoligonoDeControle;

// Limites l�gicos da �rea de desenho
Ponto Min, Max;

bool desenha = false;

Poligono Mapa, MeiaSeta, Mastro;

float angulo=0.0;

Ponto PosAtualDoMouse;
bool BotaoDown = false;

double nFrames=0;
double TempoTotal=0;

// **********************************************************************
// Imprime o texto S na posicao (x,y), com a cor 'cor'
// **********************************************************************
void printString(string S, int x, int y, int cor)
{
    defineCor(cor);
    glRasterPos3f(x, y, 0); //define posicao na tela
    for (int i = 0; i < S.length(); i++)
    {
        // GLUT_BITMAP_HELVETICA_10
        // GLUT_BITMAP_TIMES_ROMAN_24
        // GLUT_BITMAP_HELVETICA_18
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, S[i]);
    }
}
#include <sstream>
using std::stringstream;

// **********************************************************************
// Converte um nro em string
// **********************************************************************
string toString(float f)
{
    stringstream S;
    S << f;
    return S.str();
}
// **********************************************************************
// Imprime as coordenadas do ponto P na posicao (x,y), com a cor 'cor'
// **********************************************************************
void ImprimePonto(Ponto P, int x, int y, int cor)
{
    string S;
    S = "( " + toString(P.x) + ", " + toString(P.y) + ")" ;
    printString(S, x, y, cor);
}

// **********************************************************************
//  Vetor de mensagens
// **********************************************************************
string Mensagens[] = {
    "Clique o primeiro ponto.",
    "Clique o segundo ponto.",
    "Clique o terceiro ponto."
};
// **********************************************************************
//  Imprime as mensagens do programa.
//  Funcao chamada na 'display'
// **********************************************************************
void ImprimeMensagens()
{
    printString(Mensagens[nPontoAtual], -14, 13, Yellow );
    if (nPontoAtual>0)
    {
        printString("Ultimo ponto clicado: ",-14, 11, Red);
        ImprimePonto(PontosClicados[nPontoAtual-1], -3,11, Red);
    }
}

// **********************************************************************
//
// **********************************************************************
void animate()
{
    double dt;
    dt = T.getDeltaT();
    AccumDeltaT += dt;
    TempoTotal += dt;
    nFrames++;

    if (AccumDeltaT > 1.0/30) // fixa a atualiza��o da tela em 30
    {
        AccumDeltaT = 0;
        angulo+=2;
        glutPostRedisplay();
    }
    /*
    if (TempoTotal > 5.0)
    {
        cout << "Tempo Acumulado: "  << TempoTotal << " segundos. " ;
        cout << "Nros de Frames sem desenho: " << nFrames << endl;
        cout << "FPS(sem desenho): " << nFrames/TempoTotal << endl;
        TempoTotal = 0;
        nFrames = 0;
    }*/
}
// **********************************************************************
//  void reshape( int w, int h )
//  trata o redimensionamento da janela OpenGL
// **********************************************************************
void reshape( int w, int h )
{
    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Define a area a ser ocupada pela area OpenGL dentro da Janela
    glViewport(0, 0, w, h);
    // Define os limites logicos da area OpenGL dentro da Janela
    glOrtho(Min.x,Max.x, Min.y,Max.y, -10,+10);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
// **********************************************************************
// **********************************************************************
void DesenhaEixos()
{
    Ponto Meio;
    Meio.x = (Max.x+Min.x)/2;
    Meio.y = (Max.y+Min.y)/2;
    Meio.z = (Max.z+Min.z)/2;

    glBegin(GL_LINES);
    //  eixo horizontal
        glVertex2f(Min.x,Meio.y);
        glVertex2f(Max.x,Meio.y);
    //  eixo vertical
        glVertex2f(Meio.x,Min.y);
        glVertex2f(Meio.x,Max.y);
    glEnd();
}

// **********************************************************************
//
// **********************************************************************
void CarregaModelos()
{
    Mapa.LePoligono("EstadoRS.txt");
    MeiaSeta.LePoligono("MeiaSeta.txt");
    Mastro.LePoligono("Mastro.txt");
}
// **********************************************************************
//
// **********************************************************************
void CriaCurvas()
{   
    Curvas[nCurvas] = Bezier(PontosClicados[0], PontosClicados[1], PontosClicados[2]);
    nCurvas++;
}
// **********************************************************************
//
// **********************************************************************
void init()
{
    // Define a cor do fundo da tela (AZUL)
    glClearColor(0.0f, 0.0f, 0.f, 1.0f);

    CarregaModelos();
    
    CriaCurvas();
    
    float d = 15;
    Min = Ponto(-d,-d);
    Max = Ponto(d,d);
}
// **********************************************************************
//
// **********************************************************************
void DesenhaLinha(Ponto P1, Ponto P2)
{
    glBegin(GL_LINES);
        glVertex2f(P1.x,P1.y);
        glVertex2f(P2.x,P2.y);
    glEnd();
}

// **********************************************************************
// **********************************************************************
void DesenhaCurvas()
{
    for(int i=0; i<nCurvas;i++)
    {
        defineCor(Yellow);
        Curvas[i].Traca();
        defineCor(Brown);
        Curvas[i].TracaPoligonoDeControle();
    }
}
// **********************************************************************
//
// **********************************************************************
void DesenhaPontos()
{
    defineCor(IndianRed);
    glPointSize(4);
    glBegin(GL_POINTS);
    for(int i=0; i<nPontoAtual;i++)
    {
        glVertex2f(PontosClicados[i].x, PontosClicados[i].y);
        
    }
    glEnd();
    glPointSize(1);

}
// **********************************************************************
//
// **********************************************************************
void DesenhaMenu()
{
    glPushMatrix();
        glTranslated(11,13,0); // veja o arquivo MeiaSeta.txt
        MeiaSeta.desenhaPoligono();
    glPopMatrix();
}
// **********************************************************************
//  void display( void )
// **********************************************************************
void display( void )
{

	// Limpa a tela coma cor de fundo
	glClear(GL_COLOR_BUFFER_BIT);

    // Define os limites l�gicos da �rea OpenGL dentro da Janela
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	// Coloque aqui as chamadas das rotinas que desenham os objetos
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	glLineWidth(1);
	glColor3f(1,1,1); // R, G, B  [0..1]

    DesenhaMenu();
    DesenhaEixos();

    glLineWidth(3);
    glColor3f(1,0,0);
    DesenhaPontos();
    DesenhaCurvas();
    ImprimeMensagens();

	glutSwapBuffers();
}
// **********************************************************************
// ContaTempo(double tempo)
//      conta um certo n�mero de segundos e informa quanto frames
// se passaram neste per�odo.
// **********************************************************************
void ContaTempo(double tempo)
{
    Temporizador T;

    unsigned long cont = 0;
    cout << "Inicio contagem de " << tempo << "segundos ..." << flush;
    while(true)
    {
        tempo -= T.getDeltaT();
        cont++;
        if (tempo <= 0.0)
        {
            cout << "fim! - Passaram-se " << cont << " frames." << endl;
            break;
        }
    }
}
// **********************************************************************
//  void keyboard ( unsigned char key, int x, int y )
// **********************************************************************
void keyboard ( unsigned char key, int x, int y )
{

	switch ( key )
	{
		case 27:        // Termina o programa qdo
			exit ( 0 );   // a tecla ESC for pressionada
			break;
        case 't':
            ContaTempo(3);
            break;
        case ' ':
            desenha = !desenha;
        break;
		default:
			break;
	}
}
// **********************************************************************
//  void arrow_keys ( int a_keys, int x, int y )
// **********************************************************************
void arrow_keys ( int a_keys, int x, int y )
{
	switch ( a_keys )
	{
        case GLUT_KEY_LEFT:
  
            break;
        case GLUT_KEY_RIGHT:

            break;
		case GLUT_KEY_UP:       // Se pressionar UP
			glutFullScreen ( ); // Vai para Full Screen
			break;
	    case GLUT_KEY_DOWN:     // Se pressionar UP
								// Reposiciona a janela
            glutPositionWindow (50,50);
			glutReshapeWindow ( 700, 500 );
			break;
		default:
			break;
	}
}
// **********************************************************************
// Converte as coordenadas do ponto P de coordenadas de tela para
// coordenadas de universo (sistema de refer�ncia definido na glOrtho
// (ver fun��o reshape)
// Este c�digo � baseado em http://hamala.se/forums/viewtopic.php?t=20
// **********************************************************************
Ponto ConvertePonto(Ponto P)
{
    GLint viewport[4];
    GLdouble modelview[16],projection[16];
    GLfloat wx=P.x,wy,wz;
    GLdouble ox=0.0,oy=0.0,oz=0.0;
    glGetIntegerv(GL_VIEWPORT,viewport);
    P.y=viewport[3]-P.y;
    wy=P.y;
    glGetDoublev(GL_MODELVIEW_MATRIX,modelview);
    glGetDoublev(GL_PROJECTION_MATRIX,projection);
    glReadPixels(P.x,P.y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&wz);
    gluUnProject(wx,wy,wz,modelview,projection,viewport,&ox,&oy,&oz);
    return Ponto(ox,oy,oz);
}
// **********************************************************************
// Captura o clique do botao esquerdo do mouse sobre a �rea de
// desenho
// **********************************************************************
void Mouse(int button,int state,int x,int y)
{
    //cout << "Mouse..." << endl;

    if(state==GLUT_DOWN && button==GLUT_LEFT_BUTTON)
    {
        cout << "Botao ESQ Desceu" << endl;
    }
    if(state==GLUT_UP && button==GLUT_LEFT_BUTTON)
    {
        cout << "Botao ESQ Subiu" << endl;
    }
    if(state==GLUT_UP)
      return;
    if(button==GLUT_RIGHT_BUTTON)
     return;
    
    PontosClicados[nPontoAtual++] = ConvertePonto(Ponto(x,y,0));
    if (nPontoAtual==3)
    {
        nPontoAtual = 0;
        CriaCurvas();
    }
}

// **********************************************************************
// Captura as coordenadas do mouse do mouse sobre a �rea de
// desenho, enquanto um dos botoes esta sendo pressionado
// **********************************************************************
void Motion(int x, int y)
{
    Ponto P(x,y);
    PosAtualDoMouse = ConvertePonto(P);
    PosAtualDoMouse.imprime("Mouse:");
    cout << endl;
}
// **********************************************************************
//  void main ( int argc, char** argv )
//
// **********************************************************************
int  main ( int argc, char** argv )
{
    cout << "Programa OpenGL" << endl;

    glutInit            ( &argc, argv );
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
    glutInitWindowPosition (0,0);

    // Define o tamanho inicial da janela grafica do programa
    glutInitWindowSize  ( 650, 500);

    // Cria a janela na tela, definindo o nome da
    // que aparecera na barra de t�tulo da janela.
    glutCreateWindow    ( "Animacao com Bezier" );

    // executa algumas inicializa��es
    init ();

    // Define que o tratador de evento para
    // o redesenho da tela. A funcao "display"
    // ser� chamada automaticamente quando
    // for necess�rio redesenhar a janela
    glutDisplayFunc ( display );

    // Define que o tratador de evento para
    // o invalida��o da tela. A funcao "display"
    // ser� chamada automaticamente sempre que a
    // m�quina estiver ociosa (idle)
    glutIdleFunc(animate);

    // Define que o tratador de evento para
    // o redimensionamento da janela. A funcao "reshape"
    // ser� chamada automaticamente quando
    // o usu�rio alterar o tamanho da janela
    glutReshapeFunc ( reshape );

    // Define que o tratador de evento para
    // as teclas. A funcao "keyboard"
    // ser� chamada automaticamente sempre
    // o usu�rio pressionar uma tecla comum
    glutKeyboardFunc ( keyboard );

    // Define que o tratador de evento para
    // as teclas especiais(F1, F2,... ALT-A,
    // ALT-B, Teclas de Seta, ...).
    // A funcao "arrow_keys" ser� chamada
    // automaticamente sempre o usu�rio
    // pressionar uma tecla especial
    glutSpecialFunc ( arrow_keys );
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    // inicia o tratamento dos eventos
    glutMainLoop ( );

    return 0;
}
