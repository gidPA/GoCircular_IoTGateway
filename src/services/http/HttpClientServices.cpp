#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

int authenticate(const char *rvmid, const char *rvmKey, const char *url, char rvmJWT[]) {
    Serial.println("\n[HTTP Auth Init] Authenticating...");
    Serial.printf("[HTTP Auth Init] RVM ID: %s\n", rvmid);
    Serial.printf("[HTTP Auth Init] RVM key: %s\n", rvmKey);
    Serial.printf("[HTTP Auth Init] API URL: %s\n", url);

    // sslClient.setCACert(rvmCred.rootCACertificate);

    HTTPClient httpClient;

    httpClient.begin(String(url));
    httpClient.addHeader("Content-Type", "application/json");

    JsonDocument bodyObj;
    bodyObj["rvmid"] = rvmid;
    bodyObj["rvmKey"] = rvmKey;
    bodyObj.shrinkToFit();

    String body;
    serializeJson(bodyObj, body);
    Serial.println(body);

    int resCode = httpClient.POST(body);

    Serial.printf("[HTTP Auth Init] Login Response code: %d \n", resCode);

    // httpClient.POST()

    if (resCode >= 200 && resCode < 250) {
        Serial.print("\n[HTTP Auth Init] Succesfully authenticated ");
        Serial.printf("with response code %d", resCode);

        String response = httpClient.getString();
        Serial.print("\n[HTTP Auth Init] Auth Response: ");
        Serial.println(response);

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, response);

        if (error) {
            Serial.print("[HTTP Auth Init] deserializeJson() failed: ");
            Serial.println(error.c_str());
        } else {
            const char *jwtToken = doc["jwtToken"];
            // Serial.printf("\n[HTTP Auth Init] JWT: %s", jwtToken);
            strncpy(rvmJWT, jwtToken, 200);
        }
    } else if (resCode >= 400 && resCode < 500) {
        Serial.printf("\n[HTTP Auth Init] Failed to authenticate with response code %d (possible incorrect credentials)\n", resCode);
    } else if (resCode >= 500 && resCode < 600) {
        Serial.printf("\n[HTTP Auth Init] Failed to authenticate with response code %d (server error)\n", resCode);
    } else {
        Serial.printf("\n[HTTP Auth Init] Failed to authenticate with response code %d (connection lost)\n", resCode);
    }

    return resCode;
}