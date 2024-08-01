#include "rvmConfig.h"

RVMConfig rvmConfig;

void RVMConfig::getAPIUrl(char apiurl[]){
    char buffer[maxSize];

    snprintf(buffer, maxSize, "%s://%s:%s%s", httpprefix, hostname, httpPort, apiEndpoint);
    strncpy(apiurl, buffer, maxSize);
}

void RVMConfig::getMQTTUrl(char mqtturl[]){
    char buffer[maxSize];

    snprintf(buffer, maxSize, "%s://%s", mqttprefix, hostname);
    strncpy(mqtturl, buffer, maxSize);
}