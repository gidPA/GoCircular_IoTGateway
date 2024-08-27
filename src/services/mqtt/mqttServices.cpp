
#include <Arduino.h>
#include <PubSubClient.h>
#include "services/wifi/WiFiServices.h"
#include "models/TransactionState.h"
#include "rvmConfig.h"

PubSubClient mqttClient(net);

int mqttInit(
             const char *hostname,
             const char *id,
             const char *username,
             const char *jwt,
             MQTT_CALLBACK_SIGNATURE
            )
{
    Serial.print("\n[MQTT Init] Connecting to MQTT Broker...\n");
    Serial.printf("[MQTT Init] Client ID: %s\n", id);
    Serial.printf("[MQTT Init] Username: %s\n", username);
    Serial.printf("[MQTT Init] JWT: %s\n", jwt);
    Serial.printf("[MQTT Init] Broker Address: %s\n", hostname);

    mqttClient.setServer(hostname, 34787);

    int i;
    Serial.print("[MQTT Init] Connecting.");
    for (i = 0; i < 3; i++)
    {
        if (mqttClient.connect(id, username, jwt))
        {
            Serial.println("\n[MQTT Init] Connected to MQTT Broker");
            
            // mqttClient.subscribe(rvmConfig.setExchangeRateTopic);
            // mqttClient.subscribe(rvmConfig.setMemberModeTopic);

            mqttClient.subscribe("gocircular/rvm/4002/input/transaction/memberMode");
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