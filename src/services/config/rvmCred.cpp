#include <Arduino.h>
#include <string.h>

#include "rvmCred.h"

WiFiCred wifiCred;
RVMCred rvmCred;

void RVMCred::getJWT(char output[])
{
    strncpy(output, jwt, JWT_LENGTH);
}

void RVMCred::setJWT(char input[])
{
    strncpy(jwt, input, JWT_LENGTH);
}

// DELETE LATER!!!!!!
void RVMCred::previewJWT()
{
    Serial.printf("\n[RVM Cred: %s]\n", jwt);
}