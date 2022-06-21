//Codigo para a Estacao Meteorologica
//PMR3402

#include <stdio.h>
#include <string.h>

//Definições para o sistema

#define NUM_ESTADOS 3
#define NUM_EVENTOS 3

// ESTADOS
#define STANDBY   0
#define MEDICOES_REALIZADAS    1
#define DADOS_ARMAZENADOS   2

// EVENTOS
#define NENHUM_EVENTO -1
#define REALIZAR_MEDICOES        0
#define ENVIAR_DADOS        1
#define DISPONIBILIZAR_DADOS       2


// ACOES
#define NENHUMA_ACAO -1
#define A01  0
#define A02  1
#define A03  2



void setup() {
  // put your setup code here, to run once:

}

// Loop principal de controle da Estação Meteorológica
void loop() {
  // put your main code here, to run repeatedly:
  int codigoEvento;
  int codigoAcao;
  int estado;

  estado = STANDBY;

  printf ("Estação Meteorologica iniciada\n");
  for( ; ; ) {  
     codigoAcao = obterAcao(estado, codigoEvento);
     estado = obterProximoEstado(estado, codigoEvento);
     executarAcao(codigoAcao);
     printf("Estado: %d Evento: %d Acao:%d\n", estado, codigoEvento, codigoAcao);
  
  }

}


// Executa uma ação 

int executarAcao(int codigoAcao)
{
    int retval;

    retval = NENHUM_EVENTO;
    if (codigoAcao == NENHUMA_ACAO)
        return retval;

    switch(codigoAcao)
    {
    case A01:
        msg_display("A01");
        break;
    case A02:
        msg_display("A02");
        break;
    case A03:
        msg_display("A03");
        break;
    } // switch

    return retval;
} // executarAcao


//Função para iniciar a máquina de estados

void iniciaMaquinaEstados()
{
  int i;
  int j;

  for (i=0; i < NUM_ESTADOS; i++) {
    for (j=0; j < NUM_EVENTOS; j++) {
       acao_matrizTransicaoEstados[i][j] = NENHUMA_ACAO;
       proximo_estado_matrizTransicaoEstados[i][j] = i;
    }
  }
  proximo_estado_matrizTransicaoEstados[STANDBY][REALIZAR_MEDICOES] = MEDICOES_REALIZADAS;
  acao_matrizTransicaoEstados[STANDBY][REALIZAR_MEDICOES] = A01;

  proximo_estado_matrizTransicaoEstados[MEDICOES_REALIZADAS][ENVIAR_DADOS] = DADOS_ARMAZENADOS;
  acao_matrizTransicaoEstados[MEDICOES_REALIZADAS][ENVIAR_DADOS] = A02;

  proximo_estado_matrizTransicaoEstados[DADOS_ARMAZENADOS][DISPONIBILIZAR_DADOS] = STANDBY;
  acao_matrizTransicaoEstados[DADOS_ARMAZENADOS][DISPONIBILIZAR_DADOS] = A03;


} // initStateMachine


//Retorna a ação a ser executada
int obterAcao(int estado, int codigoEvento) {
  return acao_matrizTransicaoEstados[estado][codigoEvento];
}


//Retorna próximo 
int obterProximoEstado(int estado, int codigoEvento) {
  return proximo_estado_matrizTransicaoEstados[estado][codigoEvento];
}

//Função utilizada para exibir mensagens no display da Estacao
char msg_display(char msg) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println(msg);
  delay(3000);
  display.display(); 
}
