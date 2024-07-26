#ifndef RVM_CONFIG
#define RVM_CONFIG

#include <string.h>
#include <stdio.h>

struct RVMConfig{
    const char* hostname = "192.168.0.178:4001";
    const char* httpprefix = "http://";
    const char* apiEndpoint = "/api/rvm/auth";

    const int maxSize = 100;

    void getAPIUrl(char apiurl[]){
        char buffer[maxSize];

        snprintf(buffer, 100, "%s%s%s", httpprefix, hostname, apiEndpoint);
        strncpy(apiurl, buffer, maxSize);
    }
}rvmConfig;

#endif