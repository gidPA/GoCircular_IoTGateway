
#include "RVMInit.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

#include "rvmCred.h"
#include "rvmConfig.h"

WiFiClient net;
PubSubClient mqttClient(net);

// MQTTClient client;
// int connectToWiFi(const char* ssid, const char* pass);
// int authenticate(const char* rvmid, const char* rvmKey, const char* url, char rvmJWT[]);
// int connectToMQTT(const char *brokerAddress, const char *id, const char *username, const char *jwt);

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

int authenticate(const char *rvmid, const char *rvmKey, const char *url, char rvmJWT[])
{
    Serial.println("\n[HTTP Auth Init] Authenticating...");
    Serial.printf("[HTTP Auth Init] RVM ID: %s\n", rvmid);
    Serial.printf("[HTTP Auth Init] RVM key: %s\n", rvmKey);
    Serial.printf("[HTTP Auth Init] API URL: %s\n", url);

    HTTPClient httpClient;

    httpClient.begin(String(url));

    JsonDocument bodyObj;
    bodyObj["rvmid"] = rvmid;
    bodyObj["rvmKey"] = rvmKey;
    bodyObj.shrinkToFit();

    String body;
    serializeJson(bodyObj, body);

    int resCode = httpClient.POST(body);

    Serial.printf("[HTTP Auth Init] Login Response code: %d \n", resCode);

    // httpClient.POST()

    if (resCode >= 200 && resCode < 250)
    {
        Serial.print("\n[HTTP Auth Init] Succesfully authenticated ");
        Serial.printf("with response code %d", resCode);

        String response = httpClient.getString();
        Serial.print("\n[HTTP Auth Init] Auth Response: ");
        Serial.println(response);

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, response);

        if (error)
        {
            Serial.print("[HTTP Auth Init] deserializeJson() failed: ");
            Serial.println(error.c_str());
        }
        else
        {
            const char *jwtToken = doc["jwtToken"];
            // Serial.printf("\n[HTTP Auth Init] JWT: %s", jwtToken);
            strncpy(rvmJWT, jwtToken, 200);
        }
    }
    else if (resCode >= 400 && resCode < 500)
    {
        Serial.printf("\n[HTTP Auth Init] Failed to authenticate with response code %d (possible incorrect credentials)\n", resCode);
    }
    else if (resCode >= 500 && resCode < 600)
    {
        Serial.printf("\n[HTTP Auth Init] Failed to authenticate with response code %d (server error)\n", resCode);
    }
    else
    {
        Serial.printf("\n[HTTP Auth Init] Failed to authenticate with response code %d (connection lost)\n", resCode);
    }

    return resCode;
}

int connectToMQTT(const char *hostname, const char *id, const char *username, const char *jwt)
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



