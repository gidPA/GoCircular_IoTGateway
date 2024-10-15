
#include <Arduino.h>
#include <PubSubClient.h>
#include "services/wifi/WiFiServices.h"
#include "services/transaction/TransactionState.h"
#include "services/config/rvmConfig.h"
#include "services/message/messageExchangeObj.h"

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

    int i;
    Serial.print("[MQTT Init] Connecting.");
    for (i = 0; i < 3; i++)
    {
        if (mqttClient.connect(id, username, jwt))
        {
            Serial.println("\n[MQTT Init] Connected to MQTT Broker");

            mqttClient.subscribe(rvmConfig.setExchangeRateTopic);
            mqttClient.subscribe(rvmConfig.setMemberModeTopic);

            // mqttClient.subscribe("gocircular/rvm/4002/input/transaction/memberMode");
            Serial.println("[MQTT Init]Succesfully subscribed");

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

            messageExchange.sendMessageAndWait();

            char readiness[6];
            snprintf(
                readiness, 
                sizeof(readiness),
                "%d",
                messageExchange.getMemberModeReadiness()
            );
            
            mqttClient.publish(rvmConfig.setMemberModeResponseTopic, readiness);
        }
        else
        {
            Serial.printf("\n[SET_MEMBER_MODE MQTT Callback] Invalid user ID: %s\n", idchar);
            mqttClient.publish(rvmConfig.setMemberModeResponseTopic, "120");
        }
    }
}