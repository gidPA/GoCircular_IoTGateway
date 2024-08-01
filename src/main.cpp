

#include <WiFi.h>
// #include <MQTT.h>
#include <Arduino.h>
#include <MessageExchange.h>

#include "rvmCred.h"
#include "rvmConfig.h"

#include "RVMInit.h"


MessageExchange messageExchange;


unsigned long lastMillis = 0;

void handleIncomingMessage();


void setup(){
  Serial.begin(115200);
  Serial2.begin(115200);

  Serial.println("[Global] Initializing...");
  delay(2000);

  int err = 1;

  messageExchange.setUartDevice(&Serial2);
  messageExchange.setUartMonitoringDevice(&Serial);
  messageExchange.setSignature("MessageExchange");

  for (int i = 0; ((i < 3) && (err > 0)); i++){
    Serial.printf("\n[RVM Init] Step %d\n", i);
    switch(i){
      case 0:
        err = connectToWiFi(wifiCred.ssid, wifiCred.pass);
        break;
      case 1:
        char apiURL[100];

        rvmConfig.getAPIUrl(apiURL);
        err = authenticate(rvmCred.rvmid, rvmCred.secretKey, apiURL, rvmCred.jwt);

        if(err > 0 && !(err >= 400)) rvmCred.previewJWT();
        break;
      case 2:
        err = connectToMQTT(rvmConfig.hostname, rvmCred.rvmid, rvmCred.rvmid, rvmCred.jwt);
    }

    if(err <= 0){
      Serial.printf("\n[RVM Init] Step %d has failed. Init stage aborted\n", i);
    }
  }

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


