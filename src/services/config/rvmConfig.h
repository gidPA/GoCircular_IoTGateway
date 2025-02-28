#ifndef RVM_CONFIG
#define RVM_CONFIG

#include <string.h>
//#include <stdio.h>



struct RVMConfig {
    private:
        const char* mqttTopicPrefix = "gocircular/rvm";

        const char* itemPendingSubtopic = "output/pending_item";
        const char* itemEntrySubtopic = "output/entered_item";
        const char* itemRejectSubtopic = "output/rejected_item";
        const char* transactionReportSubtopic = "output/transaction_report";

        const char* rvmStatusSubtopic = "output/status";

        // const char* binFullAlertSubtopic = "output/bin_full";
        // const char* coinEmptySubtopic = "output/coin_empty";


        const char* setMemberModeResponseSubtopic = "output/member_mode_ack";
        const char* setMemberModeSubtopic = "input/member_mode_req";
        const char* setExchangeRateSubtopic = "input/set_er";

        const char* timestampMarkingSubtopic = "input/timestamp_ack";

        const int maxSize = 255;

    public:
        const char* id = "4012";
        const char* httpAddress = "http://192.168.0.178";
        const char* mqttAddress = "192.168.0.178";
        int mqttPort = 1883;
        const char* ntpServer = "192.168.0.178";
        const char* mqttprefix = "mqtt";
        int tzGMTPlus = 7;
        //const char* apiEndpoint = "/api/rvm/auth";

        char httpAuthURL[100];

        char itemPendingTopic[100];
        char itemEntryTopic[100];
        char itemRejectTopic[100];
        char transactionReportTopic[100];
        char rvmStatusTopic[100];
        // char binFullAlertTopic[100];
        // char coinEmptyTopic[100];

        char setMemberModeResponseTopic[100];
        char setMemberModeTopic[100];
        char setExchangeRateTopic[100];

        char timestampMarkingTopic[100];

        void initiate();



        //void getAPIUrl(char apiurl[]);
        
        void getMQTTUrl(char mqtturl[]);
};

extern RVMConfig rvmConfig;

#endif