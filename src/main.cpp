

#include <WiFi.h>
#include <time.h>
#include <services/time/timeHandler.h>
// #include <MQTT.h>
#include <Arduino.h>
// #include <MessageExchange.h>
#include <ArduinoJson.h>
#include <esp_timer.h>

#include "rvmCred.h"
#include "rvmConfig.h"

#include "models/TransactionState.h"

#include "services/message/messageExchangeObj.h"
#include "services/message/SetMemberModeRequest.h"

#include "services/wifi/WiFiServices.h"
#include "services/http/HttpClientServices.h"
#include "services/mqtt/mqttServices.h"

// #include "RVMInit.h"

// MessageExchange messageExchange;

unsigned long lastMillis = 0;

void printHeapInfo()
{
  Serial.printf("Total Heap: %d bytes\n", ESP.getHeapSize());
  Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
  Serial.printf("Min Free Heap: %d bytes\n", ESP.getMinFreeHeap());
  Serial.printf("Max Allocatable Heap: %d bytes\n", ESP.getMaxAllocHeap());
}

void handleIncomingMessage();
void createPendingRecyclableJson(byte recyclableData[], char messageBuffer[]);
void createRecyclableJson(byte recyclableData[], char messageBuffer[]);
void haltFirmware(const char* haltMessage);

void mqttCallback(char *topic, byte *payload, unsigned int length);

void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200);

  Serial.println("[Global] Initializing...");
  delay(2000);

  rvmConfig.initiate();

  int err = 1;

  messageExchange.setUartDevice(&Serial2);
  messageExchange.setUartMonitoringDevice(&Serial);
  messageExchange.setSignature("MessageExchange");

  for (int i = 0; ((i < 4) && (err > 0)); i++)
  {
    Serial.printf("\n[RVM Init] Step %d\n", i);
    switch (i)
    {
    case 0:
      err = connectToWiFi(wifiCred.ssid, wifiCred.pass);
      break;
    case 1:
      configTime((rvmConfig.tzGMTPlus * 3600), 0, rvmConfig.ntpServer);
      err = testNTPClient();
      break;

    case 2:
      err = authenticate(rvmCred.rvmid, rvmCred.secretKey, rvmConfig.httpAuthURL, rvmCred.jwt);

      if (err > 0 && !(err >= 400))
        rvmCred.previewJWT();
      break;


    case 3:
      err = mqttInit(rvmConfig.mqttAddress, rvmCred.rvmid, rvmCred.rvmid, rvmCred.jwt, mqttCallback);
      break;
    }

    if (err <= 0)
    {
      Serial.printf("\n[RVM Init] Step %d has failed. Init stage aborted\n", i);
      switch(i){
        case 0:{
          haltFirmware("Failed to connect to WiFi Network");
          break;
        }
        case 1:{
          haltFirmware("Failed to synchronize system time");
          break;
        }
        case 2:{
          haltFirmware("Failed to authenticate to HTTP Server");
          break;
        }
        case 3:{
          haltFirmware("Failed to connect to MQTT Server");
          break;
        }
      }


    }
  }


  

  esp_timer_create_args_t timer_args = {
    .callback = &MemberRequest::onTimer,
    .name = "Periodic UART Timer"
  };

  esp_timer_create(&timer_args, &memberModeRequestTimer);
}

void loop()
{
  if (!mqttClient.connected())
  {
    mqttInit(rvmConfig.mqttAddress, rvmCred.rvmid, rvmCred.rvmid, rvmCred.jwt, mqttCallback);
  }
  mqttClient.loop();
  if (messageExchange.getUartDevice()->available() >= MESSAGE_SIZE)
  {
    handleIncomingMessage();
  }
}

void handleIncomingMessage()
{
  byte messageTopic = messageExchange.handleIncomingMessage();
  messageExchange.previewMessage();

  switch (messageTopic)
  {
  case BEGIN_TRANSACTION:
  {
    Serial.println("[BEGIN_TRANSACTION MESSAGE HANDLER] New Transaction Initiated");
    
    transactionState.initializeTransaction();
    // transactionState.isBusy = true;

    break;
  }
  case ITEM_ENTRY:
  {
    byte entryStatus = messageExchange.getItemEntryStatus();

    if (entryStatus == STAT_PENDING)
    {
      Serial.println("[ITEM_ENTRY MESSAGE HANDLER] Incoming pending item information");
      byte data[2];
      data[0] = messageExchange.getItemType();
      data[1] = messageExchange.getItemSize();

      // Serial.printf("Data value: %s, %s", data[0], data[1]);

      char messageBuffer[100];
      createPendingRecyclableJson(data, messageBuffer);
      Serial.printf("[ITEM_ENTRY MESSAGE HANDLER] Created JSON: %s\n\n", messageBuffer);

      for (int i = 0; i < 3; i++)
      {
        bool success = mqttClient.publish(rvmConfig.itemPendingTopic, messageBuffer);
        if (success)
        {
          Serial.println("[ITEM_ENTRY MESSAGE HANDLER] Successfully Reported");
          break;
        }
        else
        {
          Serial.println("[ITEM_ENTRY MESSAGE HANDLER] Failed to report");
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

      for (int i = 0; i < 3; i++)
      {
        bool success = mqttClient.publish(rvmConfig.itemEntryTopic, messageBuffer);
        if (success)
        {
          Serial.println("[ITEM_ENTRY MESSAGE HANDLER] Successfully Reported");
          break;
        }
        else
        {
          Serial.println("[ITEM_ENTRY MESSAGE HANDLER] Failed to report");
        }
      }

      transactionState.appendNewItem(data, 3);

      transactionState.previewTotalJsonMessage();
    }

    break;
  }

  case TRANSACTION_COMPLETE:
  {
    // messageExchange.previewMessage();
    Serial.println("[TRANSACTION_COMPLETE MESSAGE HANDLER] Current transaction completed");
    transactionState.finalizeTransaction();
    break;
  }

  // case READY_FOR_TRANSACTION:
  // {
  //   Serial.println("[SET_MEMBER_MODE MQTT Callback] Response received");
  //   messageExchange.previewMessage();

  //   byte rvmReadiness = messageExchange.getMemberModeReadiness();
  //   if (rvmReadiness == MEMBER_APPROVE)
  //   {
  //     transactionState.setTransactionAsMemberMode(transactionState.memberID);
  //     mqttClient.publish(rvmConfig.setMemberModeResponseTopic, "1");
  //     Serial.println("[SET_MEMBER_MODE MQTT Callback] Succesfully set as member mode");
  //   }
  //   else
  //   {
  //     char buffer[10];
  //     itoa(rvmReadiness, buffer, 10);
  //     Serial.printf("[SET_MEMBER_MODE MQTT Callback] Setting member mode failed with code: %s\n", buffer);
  //     mqttClient.publish(rvmConfig.setMemberModeResponseTopic, buffer);
  //   }
  //   MemberRequest::responseReceived = true;
  //   break;
  // }

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

  JsonArray data = doc["enteredItem"].to<JsonArray>();
  data.add(recyclableData[0]);
  data.add(recyclableData[1]);
  data.add(recyclableData[2]);

  char output[100];

  doc.shrinkToFit(); // optional

  serializeJson(doc, output);

  strncpy(messageBuffer, output, 100);
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("[MQTT CALLBACK] Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if (strcmp(topic, rvmConfig.setMemberModeTopic) == 0)
  {
    char idchar[15];
    memcpy(idchar, payload, 15);
    idchar[length] = '\0';

    int idInt = atoi(idchar);
    if (idInt >= 1000 && idInt < 2000)
    {
      strncpy(transactionState.memberID, idchar, sizeof(transactionState.memberID));
      // MemberRequest::startPeriodicRequest();

      messageExchange.createNewMessage(SET_MEMBER_MODE);
      messageExchange.previewMessage();
      messageExchange.sendMessage();

      while(1){
        if(messageExchange.checkMessageEntry()){
          messageExchange.handleIncomingMessage();
          messageExchange.previewMessage();
          break;
        };

        delay(2000);
        messageExchange.createNewMessage(SET_MEMBER_MODE);
        messageExchange.previewMessage();
        messageExchange.sendMessage();

      }
    }
    else
    {
      Serial.printf("\n[SET_MEMBER_MODE MQTT Callback] Invalid user ID: %s\n", idchar);
      mqttClient.publish(rvmConfig.setMemberModeResponseTopic, "120");
    }
  }
}

void haltFirmware(const char* haltMessage){
  while(1){
    Serial.print("[GLOBAL] OPERATION HALTED! REASON: ");
    Serial.println(haltMessage);
    delay(2000);
  }

}