#include <Arduino.h>

/* BIBLIOTECAS NECESSÁRIAS PARA O WIFI E CONEXAO COM O BD VIA HTTPS */
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

/* BIBLIOTECA NECESSÁRIA PARA MANIPULAR JSON */
#include <ArduinoJson.h>

/* BIBLIOTECAS SENSORES TEMPERATURA "DS18B20"*/
#include <OneWire.h>
#include <DallasTemperature.h>
/* Inicializando */
/* Os sensores devem estar conectados na porta D6 da placa Wemos */
OneWire oneWire(D6);
DallasTemperature TDS18B20(&oneWire);

/* BIBLIOTECA SENSORES TEMPERATURA E HUMIDADE "DHT22" */
#include "DHT.h"
/* Definindo os pinos que serão usados e o tipo do sensor*/
#define DHTPIN D5  
#define DHTPIN1 D3  
#define DHTPIN2 D4  
#define DHTTYPE DHT22
/* Inicializando */
DHT dht(DHTPIN, DHTTYPE);
DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);

/* DEFININDO PINOS */
int pinoBoiler = D1;
int pinoBomba = D0;
int pinoLuzB1 = D2;
int pinoLuzB2 = D7;

/* WIFI FACUL */
char *networkName = "andromeda-ifc";
char *networkPassword = "shirito123";

/* WIFI CORTIÇO */
//char *networkName = "ANDROMEDA";
//char *networkPassword = "ifc@2020@";

/* HOST TUFAO */

const char* host = "192.168.1.4";
const int httpsPort = 443;

char *hostName = "https://192.168.1.4/arduino/index.php";

/* HOST TESTE JSON */
//char *hostNameGET = "http://jsonplaceholder.typicode.com/posts/1";
//char *hostNamePOST = "http://jsonplaceholder.typicode.com/posts";

const char fingerprint[] PROGMEM = "30 E3 3F EF 10 3C 2C F3 5A 80 ED 8F D8 59 1A F3 6F C9 CA 8F";
String tokenReceiveJson = "ola";
String tokenSendJson = "oi";
String *ponteiroTokenReceiveJson = &tokenReceiveJson;
String *ponteiroTokenSendJson = &tokenSendJson;

/* PARAMETROS TEMPERATURA PADRÃO - em caso de falha na conexão com o BD */
float TEMP_FALHA_AMBIENTE = 0.0;
float TEMP_FALHA_MINAMBIENTE = 0.0;
float TEMP_FALHA_MAXAMBIENTE = 0.0;
float TEMP_FALHA_MINBOILER = 0.0;
float TEMP_FALHA_MAXBOILER = 0.0;
float TEMP_FALHA_MINPISO = 0.0;
float TEMP_FALHA_MAXPISO = 0.0;

/* PARAMETROS TEMPERATURA */
float tempControleAmbiente = 0.0;
float tempMinAmbiente = 0.0;
float tempMaxAmbiente = 0.0;
float tempMinBoiler = 0.0;
float tempMaxBoiler = 0.0;
float tempMinPiso = 0.0;
float tempMaxPiso = 0.0;
float *ponteiroTempControleAmbiente = &tempControleAmbiente;
float *ponteiroTempMinAmbiente = &tempMinAmbiente;
float *ponteiroTempMaxAmbiente = &tempMaxAmbiente;
float *ponteiroTempMinBoiler = &tempMinBoiler;
float *ponteiroTempMaxBoiler = &tempMaxBoiler;
float *ponteiroTempMinPiso = &tempMinPiso;
float *ponteiroTempMaxPiso = &tempMaxPiso;

/* VARIAVEIS TEMPERATURA PISOS, BOILER, BOMBA */
float tempPiso01 = 0.0;
float tempPiso02 = 0.0;
float tempPiso03 = 0.0;
float tempPiso04 = 0.0;
float tempMediaPisos = 0.0;
float tempBomba = 0.0;
float tempBoiler = 0.0;
float *ponteiroTempPiso01 = &tempPiso01;
float *ponteiroTempPiso02 = &tempPiso02;
float *ponteiroTempPiso03 = &tempPiso03;
float *ponteiroTempPiso04 = &tempPiso04;
float *ponteiroTempMediaPisos = &tempMediaPisos;
float *ponteiroTempBomba = &tempBomba;
float *ponteiroTempBoiler = &tempBoiler;

/* VARIAVEIS TEMPERATURA AMBIENTE, INTERNO E EXTERNO */
/* Variaveis de umidade */
float umidade = 0.0;
float umidadeInterna = 0.0;
float umidadeExterna = 0.0;
float *ponteiroUmidade = &umidade;
float *ponteiroUmidadeInterna = &umidadeInterna;
float *ponteiroUmidadeExterna = &umidadeExterna;
/* Variaveis de temperatura - CELCIUS */
float temperatura = 0.0;
float temperaturaInterna = 0.0;
float temperaturaExterna = 0.0;
float *ponteiroTemperatura = &temperatura;
float *ponteiroTemperaturaInterna = &temperaturaInterna;
float *ponteiroTemperaturaExterna = &temperaturaExterna;

/* VARIAVEIS DE STATUS */
/* ligado ou desligado */
int statusBomba = 0;
int statusBoiler = 0;
int statusLuz = 0;
int *ponteiroStatusBomba = &statusBomba;
int *ponteiroStatusBoiler = &statusBoiler;
int *ponteiroStatusLuz = &statusLuz;
/* com problema ou não */
int statusBombaProblema;
int statusBoilerProblema;
int statusLuzProblema;
int *ponteiroStatusBombaProblema = &statusBombaProblema;
int *ponteiroStatusBoilerProblema = &statusBoilerProblema;
int *ponteiroStatusLuzProblema = &statusLuzProblema;

/* BAIAS */
int idBaia;
int *ponteiroIdBaia = &idBaia;

/* VARIAVEIS DE CONTROLE DE TEMPO */
/* Nessário para controle do tempo entre o funcionamento da Bomba e o Boiler */
int tempoAtualMillis = 0;
int tempoAnteriorMillis = 0;
int periodoEspera = 600000;
int *ponteiroTempoAtualMillis = &tempoAtualMillis;
int *ponteiroTempoAnteriorMillis = &tempoAnteriorMillis;
int *ponteiroPeriodoEspera = &periodoEspera;

void controle_temp_falha_conexão(){
  
  *ponteiroTempMinAmbiente = TEMP_FALHA_AMBIENTE;
  *ponteiroTempMaxAmbiente = TEMP_FALHA_MAXAMBIENTE;
  *ponteiroTempMinAmbiente = TEMP_FALHA_MINAMBIENTE;
  *ponteiroTempMaxBoiler = TEMP_FALHA_MAXBOILER;
  *ponteiroTempMinBoiler = TEMP_FALHA_MINBOILER;
  *ponteiroTempMaxPiso = TEMP_FALHA_MAXPISO;
  *ponteiroTempMinPiso = TEMP_FALHA_MINPISO;

}

void inicializaSensores(){
  
  dht.begin();
  dht1.begin();
  dht2.begin();
  TDS18B20.begin();
  
}

void inicializaPinos(){
  
  pinMode(pinoBoiler,OUTPUT);
  pinMode(pinoBomba,OUTPUT);
  pinMode(pinoLuzB1,OUTPUT);
  pinMode(pinoLuzB2,OUTPUT);
  
}

void atualizaLeituraSensores(){

  TDS18B20.requestTemperatures();  
  *ponteiroTempPiso01 = TDS18B20.getTempCByIndex(0);
  *ponteiroTempPiso02 = TDS18B20.getTempCByIndex(4);
  *ponteiroTempPiso03 = TDS18B20.getTempCByIndex(2);
  *ponteiroTempPiso04 = TDS18B20.getTempCByIndex(3);
  *ponteiroTempMediaPisos = (*ponteiroTempPiso01 + *ponteiroTempPiso02 + *ponteiroTempPiso03 + *ponteiroTempPiso04) / 4;
  *ponteiroTempBomba = TDS18B20.getTempCByIndex(1);
  *ponteiroTempBoiler = TDS18B20.getTempCByIndex(5);

  *ponteiroUmidade = dht.readHumidity();
  *ponteiroUmidadeInterna = dht1.readHumidity();
  *ponteiroUmidadeExterna = dht2.readHumidity();

  *ponteiroTemperatura = dht.readTemperature();
  *ponteiroTemperaturaInterna = dht1.readTemperature();
  *ponteiroTemperaturaExterna = dht2.readTemperature();
  
}

void atualizaFuncionamentoBaia(){

  if (*ponteiroTempMediaPisos < *ponteiroTempMinPiso){
    *ponteiroTempoAtualMillis = millis(); /* Inicia o contador de tempo para ligar o Boiler */ 
    ligarBomba();
    ligarLuzes(); /* Liga as Luzes enquanto o Boiler não está pronto para ser ligado */
    if (*ponteiroTempoAtualMillis >= *ponteiroPeriodoEspera){ /* Verifica o tempo necessário de espera para ligar o Boiler */
      ligarBoiler();
      desligarLuzes();
      *ponteiroTempoAtualMillis = 0;
    }    
  }
  else if (*ponteiroTempMediaPisos > *ponteiroTempMaxPiso){
    desligarBoiler();
    desligarBomba();
  }
  
  if (*ponteiroTemperatura < *ponteiroTempMinAmbiente){
    ligarLuzes();
  }
  else if (*ponteiroTemperatura > *ponteiroTempMaxAmbiente){
    desligarLuzes();  
  }
  
}

int ligarBomba(){
  
  if (statusBomba == 1){
    Serial.println("Bomba já está ligada!");
    return true;
  }
   
  digitalWrite(pinoBomba, LOW);
  if (digitalRead(pinoBomba) == !LOW){
    Serial.println("Problema ao ligar a bomba!");
    statusBombaProblema = 1;
    statusBomba = 0;
    return false;
  }
  Serial.println("Bomba ligada com sucesso!");
  statusBombaProblema = 0;
  statusBomba = 1;
  return true;
  
}

int desligarBomba(){
  
  if (statusBomba == 0){
    Serial.println("Bomba já está desligada!");
    return true;
  }
  
  digitalWrite(pinoBomba, HIGH);
  if (digitalRead(pinoBomba == !HIGH)){
    Serial.println("Problema ao desligar a bomba!");
    statusBombaProblema = 1;
    statusBomba = 1;
    return false;
  }
  Serial.println("Bomba desligada com sucesso!");
  statusBombaProblema = 0;
  statusBomba = 0;
  return true;
  
}

int ligarBoiler(){
  
  if (statusBoiler == 1){
    Serial.println("Boiler já está ligado!");
    return true;
  }

  digitalWrite(pinoBoiler, LOW);
  if (digitalRead(pinoBoiler == !LOW)){
    Serial.println("Problema ao ligar o Boiler!");
    statusBoilerProblema = 1;
    statusBoiler = 0;
    return false;
  }
  Serial.println("Boiler ligado com sucesso!");
  statusBoilerProblema = 0;
  statusBoiler = 1;
  return true;
  
}

int desligarBoiler(){
  
  if (statusBoiler == 0){
      return true;
  }
  
  digitalWrite(pinoBoiler, HIGH);
  if (digitalRead(pinoBoiler == !HIGH)){
    Serial.println("Problema ao desligar o Boiler!");
    statusBoilerProblema = 1;
    statusBoiler = 1;
    return false;
  }
  Serial.println("Boiler desligado com sucesso!");
  statusBoilerProblema = 0;
  statusBoiler = 0;
  return true;
  
}

int ligarLuzes(){
  
  if (idBaia == 1){
    if (statusLuz == 1){
      Serial.println("Luzes B1 já estão ligadas!");
      return true;
    }
    digitalWrite(pinoLuzB1, LOW);
    if (digitalRead(pinoLuzB1 == !LOW)){
      Serial.println("Problema ao ligar as luzes B1!");
      statusLuzProblema = 1;
      statusLuz = 0;
      return false;
    }
    Serial.println("Luzes B1 ligadas com sucesso!");
    statusLuzProblema = 0;
    statusLuz = 1;
    return true;
  }
  else if (idBaia == 2){
    if (statusLuz == 1){
      Serial.println("Luzes B2 já estão ligadas!");
      return true;
    }
    digitalWrite(pinoLuzB2, LOW);
    if (digitalRead(pinoLuzB2 == !LOW)){
      Serial.println("Problema ao ligar as luzes B2!");
      statusLuzProblema = 1;
      statusLuz = 0;
      return false;
    }
    Serial.println("Luzes B2 ligadas com sucesso!");
    statusLuzProblema = 0;
    statusLuz = 1;
    return true;
  }
  Serial.println("LIGAR LUZES: Problema na identificação da Baia!");
  
}

int desligarLuzes(){
  
  if (idBaia == 1){
    if (statusLuz == 0){
      Serial.println("Luzes B1 já estão desligadas!");
      return true;  
    }
    digitalWrite(pinoLuzB1, HIGH);
    if (digitalRead(pinoLuzB1 == !HIGH)){
      Serial.println("Problema ao desligar as luzes B1!");
      statusLuzProblema = 1;
      statusLuz = 1;
      return false;
    }
    Serial.println("Luzes B1 desligadas com sucesso!");
    statusLuzProblema = 0;
    statusLuz = 0;
  }
  else if (idBaia == 2){
    if (statusLuz == 0){
      Serial.println("Luzes B2 já estão desligadas!");
      return true;
    }
    digitalWrite(pinoLuzB2, HIGH);
    if (digitalRead(pinoLuzB2 == !HIGH)){
      Serial.println("Problema ao desligar as luzes B2!");
      statusLuzProblema = 1;
      statusLuz = 1;
      return false;
    }
    Serial.println("Luzes B2 desligadas com sucesso!");
    statusLuzProblema = 0;
    statusLuz = 0;
    return true;
  }
  Serial.println("DESLIGAR LUZES: Problema na identificação da Baia");
}

void setup(){
  
  delay(5000);

  Serial.begin(115200);

  inicializaSensores();
  inicializaPinos();

}

void loop(){
  
//  *ponteiroTempoAtualMillis = millis();
//  *ponteiroTempoAnteriorMillis = 0;
//  *ponteiroPeriodoEspera = 600000;
  
  delay(30000); /* Aguarda 30 segundos entre as coletas de dados e atualizações do sistema */

  scanNetworks();
  connectToNetwork();

  if (connectToHostGET() == false){
    controle_temp_falha_conexão();
  };
  atualizaLeituraSensores();
  
  if (idBaia == 1)/* Baia com boiler */
  {
    atualizaFuncionamentoBaia();  
  }
  else if (idBaia == 2)/* Baia sem boiler */
  {
    atualizaFuncionamentoBaia();  
  }

  connectToHostPOST();
  
  WiFi.disconnect(true);
  
}

void scanNetworks(){
  
  int numberOfNetworks = WiFi.scanNetworks();  
  
}

void connectToNetwork(){
  
  WiFi.begin(networkName, networkPassword);

  while (WiFi.status() != WL_CONNECTED){
      Serial.print("Establishing connection to: ");
      Serial.println(networkName);
      delay(5000);
    }
  Serial.print("Connection established with: ");
  Serial.println(networkName);

}

void connectToHostPOST(){
  
  /* WiFiClientSecure class to create TLS connection */
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);

  Serial.printf("Using fingerprint '%s'\n", fingerprint);
  client.setFingerprint(fingerprint);

  if (!client.connect(host, httpsPort)){
    Serial.println("connection failed");
    return;
  }
  
  HTTPClient httpsClient;

  httpsClient.begin(client, hostName);
  httpsClient.addHeader("Content-type","application/json");
  String JSON = createJson();
  Serial.print("Payload to be sent to the host: ");
  Serial.println(JSON);
  httpsClient.addHeader("Content", JSON);
  
  int httpCode = httpsClient.POST(JSON);
  
  Serial.println(httpCode);
   
  if(httpCode > 0){
    Serial.printf("[POST]code: %d\n", httpCode);  
        
    if(httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY){
      
      String response = httpsClient.getString();
      Serial.println(httpCode);
      Serial.println(response); 
    }
  }else{
      Serial.printf("[POST]...failed, error: %s", httpsClient.errorToString(httpCode).c_str());
      Serial.println();
  }
  httpsClient.end();
   
}

int connectToHostGET(){

  /* WiFiClientSecure class to create TLS connection */
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);

  Serial.printf("Using fingerprint '%s'\n", fingerprint);
  client.setFingerprint(fingerprint);

  if (!client.connect(host, httpsPort)){
    Serial.println("connection failed");
    return false;
  }
  
  HTTPClient httpsClient;

  httpsClient.begin(client, hostName);
  httpsClient.addHeader("Content-type","application/json");
  String JSON = createToken();
  httpsClient.addHeader("Content", JSON);
  Serial.println(JSON);
  int httpCode = httpsClient.POST(JSON);
  
  Serial.println(httpCode);
   
  if(httpCode > 0){
    Serial.printf("[POST]code: %d\n", httpCode);  
        
    if(httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY){
      
      String response = httpsClient.getString();
      getJson(response);
      Serial.println(httpCode);
      Serial.println(response);
      httpsClient.end();
      return true;
    }
  }else{
      Serial.printf("[POST]...failed, error: %s", httpsClient.errorToString(httpCode).c_str());
      Serial.println();
      httpsClient.end();
      return false;
  }
   
}

void getJson(String payload){
  
  const int capacity = JSON_OBJECT_SIZE(8) + 112;
  
  StaticJsonDocument<capacity> jsonDoc;
  
  DeserializationError error = deserializeJson(jsonDoc, payload);

  if (error){
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
  }
  /* Atualizando parametros temperatura */
  *ponteiroIdBaia = jsonDoc["idBaia"].as<int>();
  *ponteiroTempControleAmbiente = jsonDoc["tempControleAmbiente"].as<float>();
  *ponteiroTempMinAmbiente = jsonDoc["tempMinAmbiente"].as<float>();
  *ponteiroTempMaxAmbiente = jsonDoc["tempMaxAmbiente"].as<float>();
  *ponteiroTempMinBoiler = jsonDoc["tempMinBoiler"].as<float>();
  *ponteiroTempMaxBoiler = jsonDoc["tempMaxBoiler"].as<float>();
  *ponteiroTempMinPiso = jsonDoc["tempMinPiso"].as<float>();
  *ponteiroTempMaxPiso = jsonDoc["tempMaxPiso"].as<float>();
 
}

String createJson(){

  String jsonToSend;
  const int capacity = JSON_OBJECT_SIZE(14) + 175;

  StaticJsonDocument<capacity> jsonDoc;

  jsonDoc["idBaia"] = *ponteiroIdBaia;
  
  /* Leitura sensores temperaturas Baia */
  jsonDoc["tempPiso01"] = *ponteiroTempPiso01;
  jsonDoc["tempPiso02"] = *ponteiroTempPiso02;
  jsonDoc["tempPiso03"] = *ponteiroTempPiso03;
  jsonDoc["tempPiso04"] = *ponteiroTempPiso04;
  jsonDoc["tempMediaPisos"] = *ponteiroTempMediaPisos;
  jsonDoc["tempBoma"] = *ponteiroTempBomba;
  jsonDoc["tempBoiler"] = *ponteiroTempBoiler;
  
  /* Leitura sensores Ambiente */
  jsonDoc["umidade"] = *ponteiroUmidade;
  jsonDoc["umidadeInterna"] = *ponteiroUmidadeInterna;
  jsonDoc["umidadeExterna"] = *ponteiroUmidadeExterna;
  jsonDoc["temperatura"] = *ponteiroTemperatura;
  jsonDoc["temperaturaInterna"] = *ponteiroTemperaturaInterna;
  jsonDoc["temperaturaExterna"] = *ponteiroTemperaturaExterna;
  
  /* Status */
  jsonDoc["statusBomba"] = *ponteiroStatusBomba;
  jsonDoc["statusBoiler"] = *ponteiroStatusBoiler;
  jsonDoc["statusLuz"] = *ponteiroStatusLuz;
  jsonDoc["statusBombaProblema"] = *ponteiroStatusBombaProblema;
  jsonDoc["statusBoilerProblema"] = *ponteiroStatusBoilerProblema;
  jsonDoc["statusLuzProblema"] = *ponteiroStatusLuzProblema;
  
  serializeJson(jsonDoc, jsonToSend);

  return jsonToSend;
  
}

String createToken(){

  String tokenToSend;
  const int capacity = JSON_OBJECT_SIZE(4) + 255;

  StaticJsonDocument<capacity> jsonToken;

  jsonToken["token"] = *ponteiroTokenReceiveJson;
  
  serializeJson(jsonToken, tokenToSend);

  return tokenToSend;
  
}
