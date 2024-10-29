#ifndef ROUND_TRIP_MEASUREMENT_H
#define ROUND_TRIP_MEASUREMENT_H

#include <map>
#include <Arduino.h>
#include <stdint.h>
#include "TrackingTimestamp.h"

class RoundTripMeasurement{
    private:
        std::map<uint64_t, TrackingTimestamp> messageTimestampContainer;
    public:
        uint64_t createNewTimestamp();
        uint64_t setEndTimestamp(uint64_t timestampId, uint64_t endTimestamp);
        uint64_t getTimestampDifference(uint64_t timestampId);
        void clearTimestamps();
};

extern RoundTripMeasurement roundTripMeasurement;

#endif //ROUND_TRIP_MEASUREMENT_H