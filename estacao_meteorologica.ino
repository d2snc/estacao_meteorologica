//Codigo para a Estacao Meteorologica
//PMR3402

#include <stdio.h>
#include <string.h>
#include <Wire.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>


//Definições para a tela do display OLED
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

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

//Variáveis Globais

int codigoEvento;
int codigoAcao;
int estado;
int acao_matrizTransicaoEstados[NUM_ESTADOS][NUM_EVENTOS];
int proximo_estado_matrizTransicaoEstados[NUM_ESTADOS][NUM_EVENTOS];

// Pre declaração de funções

int executarAcao(int codigoAcao);
void iniciaMaquinaEstados();
int obterAcao(int estado, int codigoEvento);
int obterProximoEstado(int estado, int codigoEvento);
char msg_display(char msg);
void delay(int number_of_seconds);

//Setup do projeto

void setup() {
  // put your setup code here, to run once:
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  iniciaMaquinaEstados(); //Inicio minha maquina de estados 
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println("PMR3402");
  display.display();
  delay(1);  
  
}

// Loop principal de controle da Estação Meteorológica
void loop() {

  estado = STANDBY;
  codigoEvento = NENHUM_EVENTO;

  while(1) {  
     
     codigoAcao = obterAcao(estado, codigoEvento);
     estado = obterProximoEstado(estado, codigoEvento);
     executarAcao(codigoAcao);
     //printf("Estado: %d Evento: %d Acao:%d\n", estado, codigoEvento, codigoAcao);
  
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
        display.clearDisplay(); //Limpa display
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 10);
        // Display static text
        display.println("A01\n");
        display.display(); 
        delay(1);
        estado = MEDICOES_REALIZADAS;
        codigoEvento = ENVIAR_DADOS;
        break;
    case A02:
        display.clearDisplay(); //Limpa display
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 10);
        // Display static text
        display.println("A02\n");
        display.display(); 
        delay(1);
        estado = DADOS_ARMAZENADOS;
        codigoEvento = DISPONIBILIZAR_DADOS;
        break;
    case A03:
        display.clearDisplay(); //Limpa display
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 10);
        // Display static text
        display.println("A03\n");
        display.display(); 
        delay(1);
        estado = STANDBY;
        codigoEvento = REALIZAR_MEDICOES;
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



void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;
  
    // Storing start time
    clock_t start_time = clock();
  
    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds)
        ;
}
