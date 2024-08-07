#ifndef RVM_CONFIG
#define RVM_CONFIG

#include <string.h>
#include <stdio.h>

struct RVMConfig{

    const char* hostname = "192.168.0.178";
    const char* httpPort = "4001";
    const char* httpprefix = "http";
    const char* mqttprefix = "mqtt";
    const char* apiEndpoint = "/api/rvm/auth";

    const char* mqttTopicPrefix = "gocircular/rvm";
    char mqttTopicHead[50];

    const int maxSize = 100;

    void getAPIUrl(char apiurl[]);
    void getMQTTUrl(char mqtturl[]);
    void setMQTTTopicHead();
};

extern RVMConfig rvmConfig;

#endif