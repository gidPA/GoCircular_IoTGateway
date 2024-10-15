#ifndef RVM_CONFIG
#define RVM_CONFIG

#include <string.h>
//#include <stdio.h>



struct RVMConfig {
    private:
        const char* mqttTopicPrefix = "gocircular/rvm";

        const char* itemPendingSubtopic = "output/pending_item";
        const char* itemEntrySubtopic = "output/entered_item";
        const char* transactionReportSubtopic = "output/transaction_report";

        const char* binFullAlertSubtopic = "output/alert/bin_full";
        const char* coinEmptySubtopic = "output/alert/coin_empty";


        const char* setMemberModeResponseSubtopic = "output/member_mode_ack";
        const char* setMemberModeSubtopic = "input/member_mode_req";
        const char* setExchangeRateSubtopic = "input/set_er";

        const int maxSize = 255;

    public:
        const char* id = "4001";
        const char* httpAddress = "https://192.168.0.178";
        const char* mqttAddress = "192.168.0.178";
        int mqttPort = 1883;
        const char* ntpServer = "pool.ntp.org";
        const char* mqttprefix = "mqtt";
        int tzGMTPlus = 7;
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



        //void getAPIUrl(char apiurl[]);
        
        void getMQTTUrl(char mqtturl[]);
};

extern RVMConfig rvmConfig;

#endif