#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

WiFiClient net;
// WiFiClientSecure sslClient;

int connectToWiFi(const char *ssid, const char *pass)
{
    Serial.print("\n[WiFi Init] Checking wifi...");
    WiFi.begin(ssid, pass);

    for (int i = 0; i < 20; i++)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.print("\n[WiFi Init] Succesfully connected to WiFi with IP Address: ");
            Serial.println(WiFi.localIP());
            delay(6000);
            return 1;
        }
        else
        {
            Serial.print(".");
            delay(1000);
        }
    }

    Serial.println("\n[WiFi Init] Failed to Connect to WiFi AP");
    return 0;
}