//
//  Instancia.hpp
//  OpenGLTest
//
//  Created by Márcio Sarroglia Pinho on 22/09/20.
//  Copyright © 2020 Márcio Sarroglia Pinho. All rights reserved.
//


#ifndef Instancia_hpp
#define Instancia_hpp
#include <iostream>
using namespace std;

#include "Poligono.h"
typedef void TipoFuncao();

class Instancia{
public:
    //Poligono *modelo;
    Instancia();
    TipoFuncao *modelo;
    Ponto Posicao, Escala;
    float Rotacao;
    void desenha();
    void AtualizaPosicao(double tempoDecorrido);
    double r,g,b;
};


#endif /* Instancia_hpp */
