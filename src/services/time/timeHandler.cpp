#include <Arduino.h>
#include <time.h>
#include "timeHandler.h"


int testNTPClient()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo, 100000U))
    {
        Serial.println("[System Time Handler] Failed to obtain time");
        return -1;
    }

    Serial.println("[System Time Handler] Current Time:");
    Serial.println(&timeinfo, "[System Time Handler] %A, %B %d %Y %H:%M:%S");
    return 1;
}

void getCurrentTimeObject(tm* timeObject){

    if (!getLocalTime(timeObject, 100000U))
    {
        Serial.println("[System Time Handler] Failed to obtain time");
        timeObject = NULL;
    }
    Serial.println("[System Time Handler] Current Time:");
    Serial.println(timeObject, "[System Time Handler] %A, %B %d %Y %H:%M:%S");
}

void createSQLTimestamp(tm *timeObject, char *dateStringBuffer, byte dateStringSize){
    strftime(dateStringBuffer, dateStringSize, "%Y-%m-%d %H:%M:%S", timeObject);
}