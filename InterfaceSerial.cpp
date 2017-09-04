#include "InterfaceSerial.h"

// Baseado em http://playground.arduino.cc/Interfacing/CPPWindows

// Parametros da placa
#define BAUD_RATE CBR_9600
#define BYTE_SIZE 8
#define STOP_BITS ONESTOPBIT
#define PARITY NOPARITY

#define WAIT_TIME 500

#define TAMANHO_BUFFER 256
#define HEADER_TITULO 'T'
#define HEADER_DADO 'A'

#include <stdio.h>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <cctype>

using namespace std;

void InterfaceSerial::conectar() {
  if (conectado) return;

  // Tenta conectar a uma porta
  this->serial = CreateFile (porta.c_str(),
                             GENERIC_READ | GENERIC_WRITE,
                             0,
                             NULL,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL);

  // Verifica se conseguiu conectar
  if (this->serial == INVALID_HANDLE_VALUE) {
    // Se nao conseguiu, apresenta um erro
    if (GetLastError() == ERROR_FILE_NOT_FOUND) {
      throw new runtime_error ("ERRO: Porta " + this->porta +
                               " nao disponivel.");
    } else throw new
      runtime_error ("ERRO: nao foi possivel conectar.");
  } else {
    // Se conectado, define os parametros de comunicacao
    DCB dcbSerialParams = {0};

    //Tenta pegar o atual
    if (!GetCommState (this->serial, &dcbSerialParams) ) {
      throw new runtime_error ("ERRO: nao foi possivel pegar os parametros atuais da interface serial.");
    } else {
      // Define os parametros para a placa
      dcbSerialParams.BaudRate = BAUD_RATE;
      dcbSerialParams.ByteSize = BYTE_SIZE;
      dcbSerialParams.StopBits = STOP_BITS;
      dcbSerialParams.Parity = PARITY;

      // (Mantido da comunicacao com o Arduino, mas talvez desnecessario para a placa FRDM-KL25Z)
      // Setting the DTR to Control_Enable ensures that the Arduino is properly
      // reset upon establishing a connection
      dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

      // Define os parametros e verifica sua aplicacao adequada
      if (!SetCommState (serial, &dcbSerialParams) ) {
        throw new runtime_error ("ERRO: nao foi possivel definir os parametros da porta serial");
      } else {
        // Se deu tudo certo, conectou!!
        this->conectado = true;
        // Limpa o buffer
        PurgeComm (this->serial, PURGE_RXCLEAR | PURGE_TXCLEAR);
        // Espera para a placa ficar preparada
        Sleep (WAIT_TIME);
      }
    }
  }
}

int InterfaceSerial::ler (char *buffer,
                          unsigned int numeroDeCaracteres) {
  DWORD bytesLidos;
  unsigned int paraLer;

  // Usa o ClearCommError para pegar o status da porta serial
  ClearCommError (this->serial, &this->errors,
                  &this->status);

  // Verifica se ha algo para ler
  if (this->status.cbInQue > 0) {
    // Se ha, verifica se tem dado suficiente para ler. Se nao, le apenas os
    // caracteres disponiveis para evitar que a aplicacao fique esperando
    if (this->status.cbInQue > numeroDeCaracteres) {
      paraLer = numeroDeCaracteres;
    } else {
      paraLer = this->status.cbInQue;
    }

    // Tenta ler o numero de caracteres e
    // retorna o numero de bytes lidos em caso de sucesso
    if (ReadFile (this->serial, buffer, paraLer, &bytesLidos,
                  NULL) ) {
      return bytesLidos;
    }
  }

  // Se nada foi lido, ou houve um erro, retorna 0
  return 0;

}

void InterfaceSerial::desconectar() {
  if (this->conectado) {
    this->conectado = false;

    // Fecha o handler da serial
    CloseHandle (this->serial);
  }
}


void InterfaceSerial::inicializar(string porta) {
  if (conectado) {
    // Desconectado... e limpando
    desconectar();

    if (canais != NULL) {
      delete[] canais;
      quantidadeDeCanais = 0;
    }
  }

  this->porta = porta;
  conectar();

  // espera pelo cabecalho
  cout << "Aperte o botao reset da placa." << endl;

  // A: VALOR1, VALOR2, VALOR3,...
  bool encontrouHeader = false;
  char buffer[TAMANHO_BUFFER];

  do {
    int tamanho = ler (buffer, TAMANHO_BUFFER);

    if (tamanho != 0) {
      int posicaoHeader = 0;

      do {
        posicaoHeader = encontraHeader (buffer, posicaoHeader,
                                        tamanho);
      } while (posicaoHeader < tamanho
               && buffer[posicaoHeader] != HEADER_TITULO);

      if (posicaoHeader < tamanho) {
        // Encontrou o Header do titulo
        encontrouHeader = true;

        // Vector sera visto somente na Aula 11!
        vector<string>* lista = new vector<string>();

        int inicio = posicaoHeader + 2; // desprezando o ':'
        int i = inicio;

        for (; i < tamanho && buffer[i] != '\n'; i++) {
          if (buffer[i] == ',' || buffer[i] == '\r') {
            // Separadores de caracteres
            if (inicio != i) {
              // Ha algo
              string canal (buffer + inicio, i - inicio);
              lista->push_back (canal);
              inicio = i + 1;
            } else {
              // So tinha espaco ate entao
              inicio = i + 1;
            }
          } else if (buffer[i] == ' ') {
            // Desprezando
            inicio = i + 1;
          }
        }

        if (buffer[i] != '\n') {
          // O header nao veio completo
          cout << "Erro de leitura. Aperte novamente o botao de reset."
               << endl;
          encontrouHeader = false;
          canais = NULL;
        } else if (lista->size() == 0) {
          // Nao encontrou nada
          canais = NULL;
        } else {
          canais = new string[lista->size()];
          valores = new double[lista->size()]; // ja alocando
          vector<string>::iterator it = lista->begin();
          quantidadeDeCanais = 0;

          for (vector<string>::iterator it = lista->begin();
               it != lista->end(); it++) {
            canais[quantidadeDeCanais++] = *it;
          }
        }

        delete lista;
      } else {
        Sleep (WAIT_TIME);
      }
    } else {
      Sleep (WAIT_TIME);
    }
  } while (!encontrouHeader);
}

int InterfaceSerial::encontraHeader (char* buffer,
                                     int inicio, int tamanho) {
  int posicaoHeader = inicio;

  while (posicaoHeader < tamanho
         && buffer[posicaoHeader] != '\n') {
    posicaoHeader++;
  }

  // O proximo caracter eh um potencial header
  posicaoHeader++;

  return posicaoHeader;
}

double InterfaceSerial::getValor (string canal) {
  if (!conectado)
    throw new logic_error ("Necessario inicializar a interface serial antes de obter valor");

  for (int i = 0; i < quantidadeDeCanais; i++) {
    if (canais[i] == canal) {
      return valores[i];
    }
  }

  throw new runtime_error("Canal " + canal + " nao encontrado");
}

bool InterfaceSerial::atualizar() {
  if (!conectado)
    throw new logic_error ("Necessario inicializar a interface serial antes de atualizar");

  if (quantidadeDeCanais == 0)
    return false;

  // Esperando: codigo especifico para o windows
  Sleep (ESPERA);

  char buffer[TAMANHO_BUFFER];

  int tamanho = ler (buffer, TAMANHO_BUFFER);

  if (tamanho != 0) {
    int posicaoHeader = 0;

    while (posicaoHeader < tamanho
           && buffer[posicaoHeader] != HEADER_DADO) {
      // Aqui eh diferente pois nao tem os pula linha
      posicaoHeader = encontraHeader (buffer, posicaoHeader,
                                      tamanho);
    }

    if (posicaoHeader < tamanho) {
      // Encontrou o Header do dado
      int canalAtual = 0;

      int inicio = posicaoHeader + 2; // desprezando o ':'

      for (int i = inicio; i < tamanho
           && canalAtual < quantidadeDeCanais
           && buffer[i] != '\n'; i++) {
        if (buffer[i] == ',' || buffer[i] == '\r') {
          if (inicio != i) {
            // Encontrou numero
            string dado (buffer + inicio, i - inicio);

            valores[canalAtual] = atof (dado.c_str() );

            canalAtual++;
            inicio = i + 1;
          } else {
            // So tinha espaco ate entao
            inicio = i + 1;
          }
        } else if (!isdigit (buffer[i]) && buffer[i] != '-'
                   && buffer[i] != '.') {
          // Nao e um caracter valido
          inicio = i + 1;
        }
      }

      return true;
    }
  }

  return false;
}

string* InterfaceSerial::getNomeDosCanais() {
  return canais;
}

int InterfaceSerial::getQuantidadeDeCanais() {
  return quantidadeDeCanais;
}
