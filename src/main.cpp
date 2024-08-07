

#include <WiFi.h>
// #include <MQTT.h>
#include <Arduino.h>
#include <MessageExchange.h>
#include <ArduinoJson.h>

#include "rvmCred.h"
#include "rvmConfig.h"

#include "models/TransactionState.h"

#include "services/wifi/WiFiServices.h"
#include "services/http/HttpClientServices.h"
#include "services/mqtt/mqttServices.h"

// #include "RVMInit.h"

MessageExchange messageExchange;

unsigned long lastMillis = 0;

void handleIncomingMessage();
void createPendingRecyclableJson(byte recyclableData[], char messageBuffer[]);
void createRecyclableJson(byte recyclableData[], char messageBuffer[]);

void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200);

  Serial.println("[Global] Initializing...");
  delay(2000);

  int err = 1;

  rvmConfig.setMQTTTopicHead();

  messageExchange.setUartDevice(&Serial2);
  messageExchange.setUartMonitoringDevice(&Serial);
  messageExchange.setSignature("MessageExchange");

  for (int i = 0; ((i < 3) && (err > 0)); i++)
  {
    Serial.printf("\n[RVM Init] Step %d\n", i);
    switch (i)
    {
    case 0:
      err = connectToWiFi(wifiCred.ssid, wifiCred.pass);
      break;
    case 1:
      char apiURL[100];

      rvmConfig.getAPIUrl(apiURL);
      err = authenticate(rvmCred.rvmid, rvmCred.secretKey, apiURL, rvmCred.jwt);

      if (err > 0 && !(err >= 400))
        rvmCred.previewJWT();
      break;
    case 2:
      err = mqttInit(rvmConfig.hostname, rvmCred.rvmid, rvmCred.rvmid, rvmCred.jwt);
    }

    if (err <= 0)
    {
      Serial.printf("\n[RVM Init] Step %d has failed. Init stage aborted\n", i);
    }
  }
}

void loop()
{
  if(!mqttClient.connected()){
    mqttInit(rvmConfig.hostname, rvmCred.rvmid, rvmCred.rvmid, rvmCred.jwt);
  }

  mqttClient.loop();

  if (messageExchange.checkMessageEntry())
  {
    handleIncomingMessage();
  }


}

void handleIncomingMessage()
{
  byte messageTopic = messageExchange.handleIncomingMessage();

  switch (messageTopic)
  {
    case BEGIN_TRANSACTION:{
      Serial.println("[BEGIN_TRANSACTION MESSAGE HANDLER] New Transaction Initiated");
      transactionState.isBusy = true;

      break;
    }
    case ITEM_ENTRY:
    {

      //messageExchange.previewMessage();
      //Serial.println("Succesfully previewed");
      byte entryStatus = messageExchange.getItemEntryStatus();
      //Serial.println("Succesfully obtained");

      if (entryStatus == STAT_PENDING)
      {
        Serial.println("[ITEM_ENTRY MESSAGE HANDLER] Incoming pending item information");
        byte data[2];
        data[0] = messageExchange.getItemType();
        data[1] = messageExchange.getItemSize();

        //Serial.printf("Data value: %s, %s", data[0], data[1]);

        char messageBuffer[100];
        createPendingRecyclableJson(data, messageBuffer);
        Serial.printf("[ITEM_ENTRY MESSAGE HANDLER] Created JSON: %s\n\n", messageBuffer);
        
        char pendingItemTopic[50];
        snprintf(pendingItemTopic, 50, "%s/transaction/pendingItem", rvmConfig.mqttTopicHead);
        
        for (int i = 0; i < 3; i++){
          bool success = mqttClient.publish(pendingItemTopic, messageBuffer);
          if (success){
            Serial.printf("[ITEM_ENTRY MESSAGE HANDLER] Successfully Reported");
            break;
          } else{
            Serial.printf("[ITEM_ENTRY MESSAGE HANDLER] Failed to report");
          }
        }
        
      }

      else if (entryStatus == ACCEPTED)
      {
        Serial.println("[ITEM_ENTRY MESSAGE HANDLER] An item has been accepted");
        byte data[3];
        data[0] = messageExchange.getItemType();
        data[1] = messageExchange.getItemSize();
        data[2] = messageExchange.getItemPoint();
        
        char messageBuffer[100];
        createRecyclableJson(data, messageBuffer);
        Serial.printf("[ITEM_ENTRY MESSAGE HANDLER] Created JSON: %s\n\n", messageBuffer);
        
        char enteredItemTopic[50];
        snprintf(enteredItemTopic, 50, "%s/transaction/enteredItem", rvmConfig.mqttTopicHead);
        
        for (int i = 0; i < 3; i++){
          bool success = mqttClient.publish(enteredItemTopic, messageBuffer);
          if (success){
            Serial.printf("[ITEM_ENTRY MESSAGE HANDLER] Successfully Reported");
            break;
          } else{
            Serial.printf("[ITEM_ENTRY MESSAGE HANDLER] Failed to report");
          }
        }

        transactionState.appendNewItem(data, 3);

        transactionState.previewTotalJsonMessage();
      }

      break;
    }

    case TRANSACTION_COMPLETE:{
      //messageExchange.previewMessage();
      Serial.println("[TRANSACTION_COMPLETE MESSAGE HANDLER] Current transaction completed");
      transactionState.finalizeTransaction();
      break;
    }

    default:
    {
      // Serial.println("[MESSAGE HANDLER] Unknown message topic");
      // messageExchange.previewMessage();
      break;
    }
  }
}

void createPendingRecyclableJson(byte recyclableData[], char messageBuffer[])
{
  Serial.println("Creating JSON....");
  JsonDocument doc;

  JsonArray data = doc["pendingItem"].to<JsonArray>();
  data.add(recyclableData[0]);
  data.add(recyclableData[1]);

  char output[100];

  doc.shrinkToFit(); // optional

  serializeJson(doc, output);

  memcpy(messageBuffer, output, 100);
}

void createRecyclableJson(byte recyclableData[], char messageBuffer[])
{
  Serial.println("Creating JSON....");
  JsonDocument doc;

  JsonArray data = doc["pendingItem"].to<JsonArray>();
  data.add(recyclableData[0]);
  data.add(recyclableData[1]);
  data.add(recyclableData[2]);

  char output[100];

  doc.shrinkToFit(); // optional

  serializeJson(doc, output);

  strncpy(messageBuffer, output, 100);
}
