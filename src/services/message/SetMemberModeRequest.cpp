#include "services/message/messageExchangeObj.h"
#include "SetMemberModeRequest.h"

#include <Arduino.h>


esp_timer_handle_t memberModeRequestTimer;
esp_timer_handle_t memberModeTimeoutTimer;

namespace MemberRequest{
    bool responseReceived = false;
    void onTimer(void* arg)
    {
        if (!responseReceived)
        {
            Serial.println("[MEMBER REQUEST TIMER] Creating periodical request...");
            messageExchange.createNewMessage(SET_MEMBER_MODE);
            messageExchange.previewMessage();
            messageExchange.sendMessage();
        }
        else
        {
            Serial.println("[MEMBER REQUEST TIMER] Request finished");
            esp_timer_stop(memberModeRequestTimer);
            esp_timer_stop(memberModeTimeoutTimer);
        }
    }

    void onTimeout(void* arg){
        Serial.println("[MEMBER REQUEST TIMER] No response has been received from the RVM Controller");

        esp_timer_stop(memberModeRequestTimer);
        esp_timer_stop(memberModeTimeoutTimer);
    }

    void startPeriodicRequest(){
        Serial.println("[MEMBER REQUEST TIMER] Request Triggered");
        esp_timer_start_periodic(memberModeRequestTimer, 1000000); //repeat request every 1 second
        esp_timer_start_once(memberModeTimeoutTimer, 5000000); //set 5 second timeout
    }
}