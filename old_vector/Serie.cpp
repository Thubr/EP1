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
    return seq.size();
}

bool Serie::estaVazia(){
    return seq.empty();
}

void Serie::adicionar(double x, double y){
    if(seq.size() < NUMERO_MAXIMO_VALORES){
        Ponto *p = new Ponto;
        p->setX(x);
        p->setY(y);
        if(seq.empty()){
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
        seq.push_back(p);
    }
}

Ponto* Serie::getLimiteInferior(){
    return low;
}

Ponto* Serie::getLimiteSuperior(){
    return high;
}

Ponto* Serie::getPosicao(int posicao){
    if(0 <= posicao && posicao < seq.size())
        return seq.at(posicao);
    return NULL;
}

void Serie::imprimir(){
    for(vector<Ponto*>::iterator it = seq.begin(); it != seq.end(); it++)
        *it->imprimir();
}
