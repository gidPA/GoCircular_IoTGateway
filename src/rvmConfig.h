#ifndef RVM_CONFIG
#define RVM_CONFIG

#include <string.h>
#include <stdio.h>

struct RVMConfig {
    private:
        const char* mqttTopicPrefix = "gocircular/rvm";

        const char* itemPendingSubtopic = "output/transaction/pendingItem";
        const char* itemEntrySubtopic = "output/transaction/enteredItem";
        const char* transactionReportSubtopic = "output/transaction/report";

        const char* binFullAlertSubtopic = "output/alert/binFull";
        const char* coinEmptySubtopic = "output/alert/coinEmpty";


        const char* setMemberModeResponseSubtopic = "output/transaction/memberModeResponse";
        const char* setMemberModeSubtopic = "input/transaction/memberMode";
        const char* setExchangeRateSubtopic = "input/er/setER";

        const int maxSize = 255;

    public:
        const char* id = "4002";
        const char* hostname = "192.168.0.178";
        const char* httpPort = "4001";
        //const char* httpprefix = "http";
        const char* mqttprefix = "mqtt";
        //const char* apiEndpoint = "/api/rvm/auth";

        char httpAuthURL[100];

        char itemPendingTopic[100];
        char itemEntryTopic[100];
        char transactionReportTopic[100];
        char binFullAlertTopic[100];
        char coinEmptyTopic[100];

        char setMemberModeResponseTopic[100];
        char setMemberModeTopic[100];
        char setExchangeRateTopic[100];

        void initiate();



        void getAPIUrl(char apiurl[]);
        void getMQTTUrl(char mqtturl[]);
};

extern RVMConfig rvmConfig;

#endif