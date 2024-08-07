#ifndef WIFI_SERVICES_H
#define WIFI_SERVICES_H

#include <WiFi.h>

extern WiFiClient net;

int connectToWiFi(const char *ssid, const char *pass);

#endif //WIFI_SERVICES_H