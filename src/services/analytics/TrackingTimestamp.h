#ifndef TRACKING_TIMESTAMP_H
#define TRACKING_TIMESTAMP_H

#include <stdint.h>

class TrackingTimestamp{
    public:
        uint64_t startTimestamp = 0;
        uint64_t endTimestamp = 0;
        uint64_t diffTimestamp = 0;

    void createNewTrackingTimestamp(uint64_t startTimestamp){
        this->startTimestamp = startTimestamp;
    }

    uint64_t setEndTimestamp(uint64_t endTimestamp){
        this->endTimestamp = endTimestamp;
        this->diffTimestamp = (this->endTimestamp - this->startTimestamp);
        return this->diffTimestamp;
    }

    void resetTimestamp(){
        startTimestamp = 0;
        endTimestamp = 0;
        diffTimestamp = 0;
    }

};

#endif //ROUND_TRIP_MEASUREMENT_H