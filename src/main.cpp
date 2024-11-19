#include <WiFi.h>
#include <services/time/timeHandler.h>
#include <time.h>
#include <Arduino.h>
#include <ArduinoJson.h>

#include "services/config/rvmConfig.h"
#include "services/config/rvmCred.h"
#include "services/http/HttpClientServices.h"
#include "services/message/messageExchangeObj.h"
#include "services/mqtt/mqttServices.h"
#include "services/transaction/TransactionState.h"
#include "services/wifi/WiFiServices.h"
#include "services/analytics/RoundTripMeasurement.h"
#include "services/indicators/LedIndicator.h"

const int LED_BUSY_PIN = 25;
const int LED_IDLE_PIN = 26;
const int LED_ERROR_PIN = 27;



unsigned long lastMillis = 0;

// void printHeapInfo() {
//     Serial.printf("Total Heap: %d bytes\n", ESP.getHeapSize());
//     Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
//     Serial.printf("Min Free Heap: %d bytes\n", ESP.getMinFreeHeap());
//     Serial.printf("Max Allocatable Heap: %d bytes\n", ESP.getMaxAllocHeap());
// }

void handleIncomingMessage();
// void createPendingRecyclableJson(byte recyclableData[], char messageBuffer[]);
void createRecyclableJson(uint64_t recyclableData[], char messageBuffer[]);
void haltFirmware(const char* haltMessage);

LedIndicator ledIndicator(LED_BUSY_PIN, LED_IDLE_PIN, LED_ERROR_PIN);

// void mqttCallback(char *topic, byte *payload, unsigned int length);

void setup() {
    Serial.begin(115200);
    Serial2.begin(115200);

    Serial.println("[Global] Initializing...");
    delay(2000);

    rvmConfig.initiate();
    ledIndicator.setState("initializing");

    int err = 1;

    messageExchange.setUartDevice(&Serial2);
    messageExchange.setUartMonitoringDevice(&Serial);
    messageExchange.setSignature("MessageExchange");

    for (int i = 0; ((i < 4) && (err > 0)); i++) {
        Serial.printf("\n[RVM Init] Step %d\n", i);
        switch (i) {
            case 0:
                err = connectToWiFi(wifiCred.ssid, wifiCred.pass);
                break;
            case 1:
                err = authenticate(rvmCred.rvmid, rvmCred.secretKey, rvmConfig.httpAuthURL, rvmCred.jwt);

                if (err > 0 && !(err >= 400))
                    rvmCred.previewJWT();
                else{
                    err = -1;
                }
                break;

            case 2:
                err = mqttInit(rvmConfig.mqttAddress, rvmConfig.mqttPort, rvmCred.rvmid, rvmCred.rvmid, rvmCred.jwt, mqttCallback);
                break;

            case 3:
                configTime((rvmConfig.tzGMTPlus * 3600), 0, rvmConfig.ntpServer);
                err = testNTPClient();
                break;
        }

        if (err <= 0) {
            Serial.printf("\n[RVM Init] Step %d has failed. Init stage aborted\n", i);
            int errorCode = i + 3;
            ledIndicator.setState("error", errorCode);
            switch (i) {
                case 0: {
                    haltFirmware("Error 3->Failed to connect to WiFi Network");
                    break;
                }
                case 1: {
                    haltFirmware("Error 4->Failed to authenticate to HTTP Server");
                    break;
                }
                case 2: {
                    haltFirmware("Error 5->Failed to connect to MQTT Server");
                    break;
                }
                case 3: {
                    haltFirmware("Error 6->Failed to request time information");
                    break;
                }
            }
        }
    }
    ledIndicator.setState("idle");
}

void loop() {
    if (!mqttClient.connected()) {
        mqttInit(rvmConfig.mqttAddress, rvmConfig.mqttPort, rvmCred.rvmid, rvmCred.rvmid, rvmCred.jwt, mqttCallback);
    }
    mqttClient.loop();
    if (messageExchange.getUartDevice()->available()) {
        Serial.println("Incoming Message");
        handleIncomingMessage();
    }
}

void handleIncomingMessage() {
    byte messageTopic = messageExchange.handleIncomingMessage();
    messageExchange.previewMessage();

    switch (messageTopic) {
        case BEGIN_TRANSACTION: {
            ledIndicator.setState("busy");
            Serial.println("[BEGIN_TRANSACTION MESSAGE HANDLER] New Transaction Initiated");

            transactionState.initializeTransaction();
            publishMQTTStatus("ONLINE_BUSY");
            // transactionState.isBusy = true;

            break;
        }
        case ITEM_ENTRY: {
            byte entryStatus = messageExchange.getItemEntryStatus();

            if (entryStatus == STAT_PENDING) {
                Serial.println("[ITEM_ENTRY MESSAGE HANDLER] Incoming pending item information");
                uint64_t data[2];
                data[0] = messageExchange.getItemType();
                data[1] = messageExchange.getItemSize();
                data[2] = 0;

                // Serial.printf("Data value: %s, %s", data[0], data[1]);

                char messageBuffer[100];
                //Serial.printf("[ITEM_ENTRY MESSAGE HANDLER] Created JSON: %s\n\n", messageBuffer);

                for (int i = 0; i < 3; i++) {
                    data[3] = 0;
                    char messageBuffer[100];
                    createRecyclableJson(data, messageBuffer);
                    Serial.printf("[ITEM_ENTRY MESSAGE HANDLER] Created JSON: %s\n\n", messageBuffer);
                    bool success = mqttClient.publish(rvmConfig.itemPendingTopic, messageBuffer);
                    if (success) {
                        Serial.println("[ITEM_ENTRY MESSAGE HANDLER] Successfully Reported");
                        break;
                    } else {
                        Serial.println("[ITEM_ENTRY MESSAGE HANDLER] Failed to report");
                    }
                }
            }

            else if (entryStatus == ACCEPTED) {
                Serial.println("[ITEM_ENTRY MESSAGE HANDLER] An item has been accepted");
                uint64_t data[3];
                data[0] = messageExchange.getItemType();
                data[1] = messageExchange.getItemSize();
                data[2] = messageExchange.getItemPoint();

                char messageBuffer[100];

                for (int i = 0; i < 3; i++) {
                    data[3] = roundTripMeasurement.createNewTimestamp();
                    createRecyclableJson(data, messageBuffer);
                    Serial.printf("[ITEM_ENTRY MESSAGE HANDLER] Created JSON: %s\n\n", messageBuffer);
                    bool success = mqttClient.publish(rvmConfig.itemEntryTopic, messageBuffer);
                    if (success) {
                        Serial.println("[ITEM_ENTRY MESSAGE HANDLER] Successfully Reported");
                        break;
                    } else {
                        Serial.println("[ITEM_ENTRY MESSAGE HANDLER] Failed to report");
                    }
                }

                transactionState.appendNewItem(data, sizeof(data));

                transactionState.previewTotalJsonMessage();
            } else if (entryStatus == DECLINED){
                Serial.println("[ITEM_ENTRY MESSAGE HANDLER] An Item has been rejected");
                uint64_t data[2];
                data[0] = messageExchange.getItemType();
                data[1] = messageExchange.getItemSize();
                data[2] = 0;
                data[3] = 0;

                // Serial.printf("Data value: %s, %s", data[0], data[1]);

                char messageBuffer[100];
                createRecyclableJson(data, messageBuffer);
                Serial.printf("[ITEM_ENTRY MESSAGE HANDLER] Created JSON: %s\n\n", messageBuffer);

                for (int i = 0; i < 3; i++) {
                    bool success = mqttClient.publish(rvmConfig.itemRejectTopic, messageBuffer);
                    if (success) {
                        Serial.println("[ITEM_ENTRY MESSAGE HANDLER] Successfully Reported");
                        break;
                    } else {
                        Serial.println("[ITEM_ENTRY MESSAGE HANDLER] Failed to report");
                    }
                }
            }

            break;
        }

        case TRANSACTION_COMPLETE: {
            Serial.println("[TRANSACTION_COMPLETE MESSAGE HANDLER] Current transaction completed");
            transactionState.finalizeTransaction(rvmConfig.transactionReportTopic);
            publishMQTTStatus("ONLINE_IDLE");
            ledIndicator.setState("idle");
            break;
        }

        default: {
            // Serial.println("[MESSAGE HANDLER] Unknown message topic");
            // messageExchange.previewMessage();
            break;
        }
    }
}

// void createPendingRecyclableJson(byte recyclableData[], char messageBuffer[], uint64_t timestampId) {
//     Serial.println("Creating JSON....");
//     JsonDocument doc;

//     JsonArray data = doc["enteredItem"].to<JsonArray>();
//     data.add(recyclableData[0]);
//     data.add(recyclableData[1]);

//     char output[100];

//     doc.shrinkToFit();  // optional

//     serializeJson(doc, output);

//     memcpy(messageBuffer, output, 100);
// }

void createRecyclableJson(uint64_t recyclableData[], char messageBuffer[]) {
    Serial.println("Creating JSON....");
    JsonDocument doc;

    JsonArray data = doc["enteredItem"].to<JsonArray>();
    data.add(recyclableData[0]);
    data.add(recyclableData[1]);
    data.add(recyclableData[2]);
    data.add(recyclableData[3]);

    char output[100];

    doc.shrinkToFit();  // optional

    serializeJson(doc, output);

    strncpy(messageBuffer, output, 100);
}

void haltFirmware(const char* haltMessage) {
    while (1) {
        Serial.print("[GLOBAL] OPERATION HALTED! REASON: ");
        Serial.println(haltMessage);
        delay(2000);
    }
}