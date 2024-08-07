

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
void createRecyclableJson(byte recyclableData[], char messageBuffer[]);

void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200);

  Serial.println("[Global] Initializing...");
  delay(2000);

  int err = 1;

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

        char messageBuffer[15];
        createRecyclableJson(data, messageBuffer);
        Serial.printf("[ITEM_ENTRY MESSAGE HANDLER] Created JSON: %s\n\n", messageBuffer);
      }

      else if (entryStatus == ACCEPTED)
      {
        Serial.println("[ITEM_ENTRY MESSAGE HANDLER] An item has been accepted");
        byte data[3];
        data[0] = messageExchange.getItemType();
        data[1] = messageExchange.getItemSize();
        data[2] = messageExchange.getItemPoint();

        transactionState.appendNewItem(data, 3);
        transactionState.previewTotalJsonMessage();
      }
    }

    default:
    {
      // Serial.println("[MESSAGE HANDLER] Unknown message topic");
      // messageExchange.previewMessage();
      break;
    }
  }
}

void createRecyclableJson(byte recyclableData[], char messageBuffer[])
{
  JsonDocument doc;

  JsonArray data = doc["data"].to<JsonArray>();
  data.add(recyclableData[0]);
  data.add(recyclableData[1]);

  char output[15];

  doc.shrinkToFit(); // optional

  serializeJson(doc, output);

  memcpy(messageBuffer, output, 15);
}
