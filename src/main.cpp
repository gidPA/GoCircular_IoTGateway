#include <provisionalCred.h>

#include <WiFi.h>
#include <MQTT.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <MessageExchange.h>
#include <HTTPClient.h>

MessageExchange messageExchange;
WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

void handleIncomingMessage();
int connectToWiFi(const char* ssid, const char* pass);
int authenticate(const char* rvmid, const char* rvmKey, const char* url);

void setup(){
  Serial.begin(115200);
  Serial2.begin(115200);

  messageExchange.setUartDevice(&Serial2);
  messageExchange.setUartMonitoringDevice(&Serial);
  messageExchange.setSignature("MessageExchange");

  int connectWifi = connectToWiFi(wifiCred.ssid, wifiCred.pass);
  // if (connectWifi != 0){
  //   authenticate();
  // }
}

void loop(){
  if(messageExchange.checkMessageEntry()){
    handleIncomingMessage();
  }
}

void handleIncomingMessage(){
  byte messageTopic = messageExchange.handleIncomingMessage();
  messageExchange.previewMessage();
}

int connectToWiFi(const char* ssid, const char* pass){
  Serial.print("\n[WiFi Init] Checking wifi...");
  WiFi.begin(ssid, pass);

  for (int i = 0; i < 20; i++){
    if (WiFi.status() == WL_CONNECTED){
      Serial.print("\n[WiFi Init] Succesfully connected to WiFi with IP Address: ");
      Serial.println(WiFi.localIP());
      return 1;
    }
    else {
      Serial.print(".");
      delay(1000);
    }
  }

  Serial.println("[WiFi Init] Failed to Connect to WiFi AP");
  return 0;

  

}

int authenticate(const char* rvmid, const char* rvmKey, const char* url){
  Serial.print("[HTTP Auth Init] Authenticating...");
  HTTPClient httpClient;
  
  httpClient.begin(String(url));
  
  JsonDocument bodyObj;
  bodyObj["rvmid"] = rvmid;
  bodyObj["rvmKey"] = rvmKey;
  bodyObj.shrinkToFit();

  String body;
  serializeJson(bodyObj, body);

  int resCode = httpClient.POST(body);

  Serial.printf("[HTTP Auth Init] Login Response code: %d \n", resCode);

  //httpClient.POST()
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  if (resCode >= 200 && resCode < 250){
    Serial.print("\n[HTTP Auth Init] Succesfully authenticated ");
    Serial.printf("with response code %d", resCode);
    String response = httpClient.getString();
    Serial.print("[HTTP Auth Init] Auth Response:");
    Serial.println(response);
  }
  else if (resCode >= 400 && resCode < 500){
    Serial.printf("\n[HTTP Auth Init] Failed to authenticate with response code %d (possible incorrect credentials)\n", resCode);
  }
  else if (resCode >= 500 && resCode < 600){
    Serial.printf("\n[HTTP Auth Init] Failed to authenticate with response code %d (server error)\n", resCode);
  }
  else {
    Serial.printf("\n[HTTP Auth Init] Failed to authenticate with response code %d (connection lost)\n", resCode);
  }

  return resCode;
}

void connectToMQTT(){
  Serial.print("\nconnecting...");
  while (!client.connect("arduino", "public", "public")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("/hello");
}

//087735344292