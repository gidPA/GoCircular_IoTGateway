#ifndef WIFI_SERVICES_H
#define WIFI_SERVICES_H

#include <WiFi.h>
// #include <WiFiClientSecure.h>

extern WiFiClient net;
// extern WiFiClientSecure sslClient;

int connectToWiFi(const char *ssid, const char *pass);

#endif //WIFI_SERVICES_H