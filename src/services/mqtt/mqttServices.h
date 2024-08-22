#ifndef MQTT_SERVICES_H
#define MQTT_SERVICES_H

#include <Arduino.h>
#include <PubSubClient.h>

extern PubSubClient mqttClient;



void mqttCallback(char* topic, byte* payload, unsigned int length);
int mqttInit(const char *hostname, const char *id, const char *username, const char *jwt, MQTT_CALLBACK_SIGNATURE);

#endif //MQTT_SERVICES_H