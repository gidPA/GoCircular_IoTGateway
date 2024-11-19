
#include <Arduino.h>
#include <PubSubClient.h>

#include "mqttServices.h"

#include "services/wifi/WiFiServices.h"
#include "services/transaction/TransactionState.h"
#include "services/config/rvmConfig.h"
// #include "services/message/messageExchangeObj.h"
#include "services/transaction/memberModeConfirm.h"
#include "services/analytics/RoundTripMeasurement.h"

PubSubClient mqttClient(net);

int mqttInit(
    const char *hostname,
    int port,
    const char *id,
    const char *username,
    const char *jwt,
    MQTT_CALLBACK_SIGNATURE)
{
    Serial.print("\n[MQTT Init] Connecting to MQTT Broker...\n");
    Serial.printf("[MQTT Init] Client ID: %s\n", id);
    Serial.printf("[MQTT Init] Username: %s\n", username);
    Serial.printf("[MQTT Init] JWT: %s\n", jwt);
    Serial.printf("[MQTT Init] Broker Address: %s\n", hostname);

    mqttClient.setServer(hostname, port);

    char lastWillMessageBuffer[150];
    snprintf(lastWillMessageBuffer, sizeof(lastWillMessageBuffer), 
        "{\"rvmid\": \"%s\", \"status\":\"%s\"}"
        , rvmConfig.id, "OFFLINE"
    );

    int i;
    Serial.print("[MQTT Init] Connecting.");
    for (i = 0; i < 3; i++)
    {
        if (mqttClient.connect(id, username, jwt, rvmConfig.rvmStatusTopic, 1, true, lastWillMessageBuffer))
        {
            Serial.println("\n[MQTT Init] Connected to MQTT Broker");

            mqttClient.subscribe(rvmConfig.setExchangeRateTopic);
            mqttClient.subscribe(rvmConfig.setMemberModeTopic);
            mqttClient.subscribe(rvmConfig.timestampMarkingTopic);

            // mqttClient.subscribe("gocircular/rvm/4002/input/transaction/memberMode");
            Serial.println("[MQTT Init]Succesfully subscribed");

            publishMQTTStatus("ONLINE_IDLE");
            mqttClient.setCallback(callback);

            return 1;
        }
        else
        {
            Serial.print(".");
            delay(1000);
        }
    }

    Serial.println("\n[MQTT Init] Failed to connect to MQTT Broker");
    return 0;
}

bool publishMQTTStatus(const char* status){
    char messageBuffer[150];
    snprintf(messageBuffer, sizeof(messageBuffer), 
        "{\"rvmid\":\"%s\", \"status\":\"%s\"}"
        , rvmConfig.id, status
    );
    return mqttClient.publish(rvmConfig.rvmStatusTopic, messageBuffer, true);
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
            transactionState.setTransactionAsMemberMode(idchar);
        }
        else
        {
            Serial.printf("\n[SET_MEMBER_MODE MQTT Callback] Invalid user ID: %s\n", idchar);
            mqttClient.publish(rvmConfig.setMemberModeResponseTopic, "120");
        }
    } else if(strcmp(topic,rvmConfig.timestampMarkingTopic) == 0){
        char idchar[30];
        memcpy(idchar, payload, sizeof(idchar));
        idchar[length] = '\0';

        uint64_t timestampID = strtoull(idchar, nullptr, 10);
        Serial.printf("[Round Trip Measurement] Timestamp ID: %llu \n", timestampID);

        uint64_t timestampDiff = roundTripMeasurement.setEndTimestamp(timestampID, micros());
        
        Serial.printf("[Round Trip Measurement] Current Message Latency: %llu\n", timestampDiff);
    }
}