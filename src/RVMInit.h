#ifndef RVM_INIT_H
#define RVM_INIT_H

#include <WiFi.h>
#include <PubSubClient.h>

#include "rvmCred.h"
#include "rvmConfig.h"

extern WiFiClient net;
extern PubSubClient mqttClient;

// MQTTClient client;
// int connectToWiFi(const char* ssid, const char* pass);
// int authenticate(const char* rvmid, const char* rvmKey, const char* url, char rvmJWT[]);
// int connectToMQTT(const char *brokerAddress, const char *id, const char *username, const char *jwt);

int connectToWiFi(const char *ssid, const char *pass);

int authenticate(const char *rvmid, const char *rvmKey, const char *url, char rvmJWT[]);

int connectToMQTT(const char *hostname, const char *id, const char *username, const char *jwt);

#endif


