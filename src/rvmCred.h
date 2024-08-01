#ifndef PROVISIONAL_CRED
#define PROVISIONAL_CRED

#define JWT_LENGTH 255

#include <Arduino.h>
#include <string.h>

struct WiFiCred
{
    const char* ssid = "ADVAN MF01-7682FE";
    const char* pass = "83938D57";
};

extern WiFiCred wifiCred;

struct RVMCred
{
    const char* rvmid = "4002";
    const char* secretKey = "12345";
    char jwt[255];
    
    void getJWT(char output[]);
    void setJWT(char input[]);

    //DELETE LATER!!!!!!
    void previewJWT();
};

extern RVMCred rvmCred;

#endif