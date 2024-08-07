#include <PubSubClient.h>
#include <Arduino.h>
#include "services/wifi/WiFiServices.h"

PubSubClient mqttClient(net);

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

int mqttInit(const char *hostname, const char *id, const char *username, const char *jwt)
{
    Serial.print("\n[MQTT Init] Connecting to MQTT Broker...\n");
    Serial.printf("[MQTT Init] Client ID: %s\n", id);
    Serial.printf("[MQTT Init] Username: %s\n", username);
    Serial.printf("[MQTT Init] JWT: %s\n", jwt);

    IPAddress brokerIP;
    brokerIP.fromString(hostname);

    Serial.print("[MQTT Init] Previewing IP Address once again, just to make sure: ");
    Serial.println(brokerIP.toString());

    mqttClient.setServer(brokerIP, 1883);

    int i;
    Serial.print("[MQTT Init] Connecting.");
    for (i = 0; i < 3; i++)
    {
        if (mqttClient.connect(id, username, jwt))
        {
            Serial.println("\n[MQTT Init] Connected to MQTT Broker");

            char topic[30];
            snprintf(topic, 30, "gocircular/rvm/%s/command", id);
            mqttClient.subscribe(topic);

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