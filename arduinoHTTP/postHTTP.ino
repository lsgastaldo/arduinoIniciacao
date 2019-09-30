#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

/* WIFI FACUL */
char *networkName = "andromeda-ifc";
char *networkPassword = "shirito123";

/* WIFI CORTIÇO */
//char *networkName = "ANDROMEDA";
//char *networkPassword = "ifc@2020@";

/* HOST TUFAO */
char *hostNameGET = "http://192.168.1.6/arduino/index.php";
char *hostNamePOST = "http://192.168.1.6/arduino/index.php";

/* HOST TESTE JSON */
//char *hostNameGET = "http://jsonplaceholder.typicode.com/posts/1";
//char *hostNamePOST = "http://jsonplaceholder.typicode.com/posts";


String tokenReceiveJson = "ola";
String tokenSendJson = "oi";
int quantidade_porcos = 7;
int temperatura = 23;
int semanas = 4;

String *ponteiroTokenReceiveJson = &tokenReceiveJson;
String *ponteiroTokenSendJson = &tokenSendJson;
int *ponteiroQuantidade_porcos = &quantidade_porcos;
int *ponteiroTemperatura = &temperatura;
int *ponteiroSemanas = &semanas;

void setup(){
  
  delay(5000);

  Serial.begin(115200);

  
  Serial.print("Temperatura antes GET: ");
  Serial.println(temperatura);

  Serial.print("Semanas antes GET: ");
  Serial.println(semanas);

  Serial.print("Qtd porcos antes GET: ");
  Serial.println(quantidade_porcos);
  
  scanNetworks();
  connectToNetwork();
  // connectToHostGET();
  connectToHostGetJson();
  connectToHostPOST();
  
  Serial.print("Temperatura depois GET: ");
  Serial.println(temperatura);

  Serial.print("Semanas depois GET: ");
  Serial.println(semanas);

  Serial.print("Qtd porcos depois GET: ");
  Serial.println(quantidade_porcos);
  
  WiFi.disconnect(true);
  
}

void loop(){
  
  delay(60000);
  setup();
  
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

void connectToHostGET(){

  HTTPClient client;

  client.begin(hostNameGET);
  
  int httpCode = client.GET();
  Serial.println(httpCode);

  if(httpCode > 0 ){
      Serial.printf("[GET]code: %d\n", httpCode);  
          
      if(httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY){
        String payload = client.getString();
        Serial.print("Payload received from the host: ");
        Serial.println(payload);
        getObjJson(payload);
      }
   }else{
      Serial.printf("[GET]...failed, error: %s", client.errorToString(httpCode).c_str());
      Serial.println();
  }
  client.end();
     
}

void getObjJson(String payload){
  
  const int capacity = JSON_ARRAY_SIZE(3) + 3*JSON_OBJECT_SIZE(4) + 255;
  
  StaticJsonDocument<capacity> jsonDoc;
  
  DeserializationError error = deserializeJson(jsonDoc, payload);

  if (error){
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
  }
  
   *ponteiroTemperatura = jsonDoc["temperatura"].as<int>();
   *ponteiroSemanas = jsonDoc["semanas"].as<int>();
   *ponteiroQuantidade_porcos = jsonDoc["quantidade_porcos"].as<int>();

}

void connectToHostPOST(){
  
  HTTPClient client;

  client.begin(hostNamePOST);
  client.addHeader("Content-type","application/json");
  String JSON = createObjJson();
  Serial.print("Payload to be sent to the host: ");
  Serial.println(JSON);
  client.addHeader("Content", JSON);
  
  int httpCode = client.POST(JSON);
  
  Serial.println(httpCode);
   
  if(httpCode > 0){
    Serial.printf("[POST]code: %d\n", httpCode);  
        
    if(httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY){
      
      String response = client.getString();
      Serial.println(httpCode);
      Serial.println(response); 
    }
   }else{
      Serial.printf("[POST]...failed, error: %s", client.errorToString(httpCode).c_str());
      Serial.println();
   }
   client.end();
}

String createObjJson(){

  String jsonToSend;
  const int capacity = JSON_OBJECT_SIZE(4) + 255;

  StaticJsonDocument<capacity> jsonDoc;

  jsonDoc["temperatura"] = *ponteiroTemperatura;
  jsonDoc["semanas"] = *ponteiroSemanas;
  jsonDoc["quantidade_porcos"] = *ponteiroQuantidade_porcos;
  jsonDoc["token"] = *ponteiroTokenSendJson;
  
  serializeJson(jsonDoc, jsonToSend);

  return jsonToSend;
}

void connectToHostGetJson(){

  HTTPClient client;

  client.begin(hostNamePOST);
  client.addHeader("Content-type","application/json");
  String JSON = createToken();
  client.addHeader("Content", JSON);
  Serial.println(JSON);
  int httpCode = client.POST(JSON);
  
  Serial.println(httpCode);
   
  if(httpCode > 0){
    Serial.printf("[POST]code: %d\n", httpCode);  
        
    if(httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY){
      
      String response = client.getString();
      getObjJson(response);
      Serial.println(httpCode);
      Serial.println(response);

    }
   }else{
      Serial.printf("[POST]...failed, error: %s", client.errorToString(httpCode).c_str());
      Serial.println();
   }
   client.end();
}

String createToken(){

  String tokenToSend;
  const int capacity = JSON_OBJECT_SIZE(4) + 255;

  StaticJsonDocument<capacity> jsonToken;

  jsonToken["token"] = *ponteiroTokenReceiveJson;
  
  serializeJson(jsonToken, tokenToSend);

  return tokenToSend;
}
