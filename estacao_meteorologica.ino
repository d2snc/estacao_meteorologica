//Codigo para a Estacao Meteorologica
//PMR3402

#include <stdio.h>
#include <string.h>
#include <Wire.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>
#include <Adafruit_BMP280.h>
#include <WiFi.h>
#include <HTTPClient.h>


//Definições para o sensor BMP280

#define BMP_SDA 21
#define BMP_SCL 22

//Definições para o Wi-fi
const char* ssid = "AP 104_EXT";
const char* password = "11043083";

//Configurações do servidor
//const char* serverName = "http://estacao-meteorologica.000webhostapp.com/esp-dados-post.php";

//Configurações para comunicação com o servidor
String apiKeyValue = "USPPMR3402";
String sensorName = "ESTAC";
String sensorLocation = "Casa";

IPAddress staticIP(192, 168, 15, 35);
IPAddress gateway(192, 168, 15, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);   // dns do google
IPAddress secondaryDNS(8, 8, 4, 4); // 


//Definições para a tela do display OLED
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//Definições para o sensor DHT
#define DHT_SENSOR_PIN 2 // ESP32 pin GIOP2 connected to DHT11 sensor
#define DHT_SENSOR_TYPE DHT11
DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);


// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//Definições para o sensor BMP280
Adafruit_BMP280 bmp280;

//Definição para o sensor de chuva FC-37
#define rainAnalog 34

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
float humi;
float tempC;
float pressao;
int rainAnalogVal;
float altitude;

// Pre declaração de funções

int executarAcao(int codigoAcao);
void iniciaMaquinaEstados();
int obterAcao(int estado, int codigoEvento);
int obterProximoEstado(int estado, int codigoEvento);
char msg_display(char msg);
void delay(int number_of_seconds);

//Setup do projeto

void setup() {
  //Inicia serial
  Serial.begin(115200);
  //Config Wi-fi
  // Configures static IP address
  if (!WiFi.config(staticIP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }
  //Inicializa o sensor BMP280
  bmp280.begin(0x76);
  //Inicializa o sensor DHT
  dht_sensor.begin(); // initialize the DHT sensor
  // Inicializa o display
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
  display.println("PMR3402\n");
  display.display();
  delay(1);  
  
  //Conecta no Wifi
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(1);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP()); 
  
}

// Loop principal de controle da Estação Meteorológica
void loop() {
  // Leitura da humidade
  humi  = dht_sensor.readHumidity();
  // Leitura da temperatura em graus celsius
  tempC = dht_sensor.readTemperature();
  // Leitura da pressao
  pressao = bmp280.readPressure() / 100;
  //Leitura da chuva
  rainAnalogVal = analogRead(rainAnalog);
  //Leitura da Altitude
  altitude = bmp280.readAltitude(1023); //1023 é a pressao atmosferica no nivel do mar para SP
  
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
        display.setCursor(0, 0);
        // Display static text
        display.println("A01 - MEDICOES\n");
        display.setCursor(0, 10);
        display.printf("Temperatura: %5.2f C\r\n", tempC);
        display.printf("Humidade : %5.2f %%\r\n", humi);
        display.printf("Pressao : %5.2f hPa\r\n", pressao);
        display.printf("Chuva : %d \r\n", rainAnalogVal);
        display.printf("Altitude : %5.2f m\r\n", altitude);
        display.drawRect(109, 24, 3, 3, WHITE); // put degree symbol ( ° )
        display.display(); 
        delay(5);
        estado = MEDICOES_REALIZADAS; //Proximo estado
        codigoEvento = ENVIAR_DADOS; //Proximo evento
        break;
    case A02:
        display.clearDisplay(); //Limpa display
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 10);
        // Display static text
        display.println("A02 - ENVIAR\n");
        display.display(); 
        

        if(WiFi.status()== WL_CONNECTED){ 
                 
          HTTPClient http;
      
          http.begin("http://estacao-meteorologica.000webhostapp.com/esp-dados-post.php");
          
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");
          
          // POST Request
          String httpRequestData = "sensor=" + sensorName + "&local=" + sensorLocation + "&temperatura=" + String(tempC) + "&humidade=" + String(humi) + "&pressao=" + String(pressao) + "&chuva=" + String(rainAnalogVal) + "&altitude=" + String(altitude) + "";
  
          int httpResponseCode = http.POST(httpRequestData);
          //int httpResponseCode = http.GET();
          Serial.print(httpRequestData);
          Serial.print("\nHTTP Response code: ");
          Serial.println(httpResponseCode);
          Serial.print("\nResponse: \n");
          String response = http.getString();
          Serial.println(response);
  
          http.end();
        } 
        else {
          Serial.println("Wifi Disconnected");
        }
        delay(5);
        estado = DADOS_ARMAZENADOS; //Proximo estado
        codigoEvento = DISPONIBILIZAR_DADOS; //Proximo evento
        break;
    case A03:
        display.clearDisplay(); //Limpa display
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 10);
        // Display static text
        display.println("A03 - DISPONIBILIZAR\n");
        display.display(); 
        delay(5);
        estado = STANDBY; //Proximo estado
        codigoEvento = REALIZAR_MEDICOES; //Proximo evento
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
