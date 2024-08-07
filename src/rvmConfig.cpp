#include "rvmConfig.h"
#include "rvmCred.h"

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

void RVMConfig::setMQTTTopicHead(){
    snprintf(mqttTopicHead, 50, "%s/%s", mqttTopicPrefix, rvmCred.rvmid);
    Serial.println(mqttTopicHead);
}