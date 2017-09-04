#include <iostream>

#include "Serie.h"
#include "Ponto.h"
#include "InterfaceSerial.h"

#define COMM "\\\\.\\COM3"

using namespace std;

int main() {
  InterfaceSerial* is = new InterfaceSerial();
  is->inicializar(COMM);

  // Obtem o nome da Serie e os canais escolhidos
  ...

  // Obtem o numero de Pontos a adicionar
  int quantidade;
  ...

  // Cria a Serie
  ...

  cout << "Obtendo os pontos" << endl;
  for (int i = 0; i < quantidade; i++) {
    is->atualizar();
    // adiciona os valores a Serie
    ...
  }

  cout << "Imprimindo os pontos obtidos" << endl;
  ...

  cout << "Limite Superior: ";
  ...

  cout << "Limite Inferior: ";
  ...
  return 0;
}
