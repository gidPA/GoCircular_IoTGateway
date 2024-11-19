#ifndef MQTT_SERVICES_H
#define MQTT_SERVICES_H

#include <Arduino.h>
#include <PubSubClient.h>

extern PubSubClient mqttClient;

void mqttCallback(char *topic, byte *payload, unsigned int length);
int mqttInit(const char *hostname, int port, const char *id, const char *username, const char *jwt, MQTT_CALLBACK_SIGNATURE);
bool publishMQTTStatus(const char* status);
#endif //MQTT_SERVICES_H