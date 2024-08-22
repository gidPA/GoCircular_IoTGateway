#ifndef SET_MEMBER_MODE_REQUEST_H
#define SET_MEMBER_MODE_REQUEST_H

#include <esp_timer.h>

namespace MemberRequest{
    extern bool responseReceived;
    void onTimer(void* arg);
    void startPeriodicRequest();
};

extern esp_timer_handle_t memberModeRequestTimer;
extern esp_timer_handle_t memberModeTimeoutTimer;

#endif //SET_MEMBER_MODE_REQUEST_H