#include "Serie.h"
using namespace std;

void Serie::setNome(string nome){
    id = nome;
}

void Serie::setNomeDoCanalX(string nomeDoCanalX){
    idX = nomeDoCanalX;
}

void Serie::setNomeDoCanalY(string nomeDoCanalY){
    idY = nomeDoCanalY;
}

string Serie::getNome(){
    return id;
}

string Serie::getNomeDoCanalX(){
    return idX;
}

string Serie::getNomeDoCanalY(){
    return idY;
}

int Serie::getQuantidade(){
    return qnt;
}

bool Serie::estaVazia(){
    return qnt?false:true;
}

void Serie::adicionar(double x, double y){
    if(qnt < NUMERO_MAXIMO_VALORES){
        Ponto *p = new Ponto;
        p->setX(x);
        p->setY(y);
        if(qnt == 0){
            *low = *p;
            *high = *p;
        }
        else{
            if(x < low->getX())
                low->setX(x);
            if(y < low->getY())
                low->setY(y);
            if(x > high->getX())
                high->setX(x);
            if(y > high->getY())
                high->setY(y);
        }
        seq[qnt] = p;
        qnt++;
    }
}

Ponto* Serie::getLimiteInferior(){
    return low;
}

Ponto* Serie::getLimiteSuperior(){
    return high;
}

Ponto* Serie::getPosicao(int posicao){
    if(0 <= posicao && posicao < qnt)
        return seq[posicao];
    return NULL;
}

void Serie::imprimir(){
    for(int i = 0; i < qnt; i++)
        *seq[i]->imprimir();
}
