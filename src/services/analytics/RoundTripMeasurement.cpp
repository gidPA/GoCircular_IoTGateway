#include <Arduino.h>
#include "RoundTripMeasurement.h"

uint64_t RoundTripMeasurement::createNewTimestamp(){
    uint64_t timestampID = micros();
    Serial.printf("[Round Trip Measurement] Created Timestamp ID: %llu \n", timestampID);
    TrackingTimestamp newTrackingTimestamp;
    newTrackingTimestamp.createNewTrackingTimestamp(timestampID);

    messageTimestampContainer.emplace(timestampID, newTrackingTimestamp);

    return timestampID;
}

uint64_t RoundTripMeasurement::setEndTimestamp(uint64_t timestampId, uint64_t endTimestamp){
    return this->messageTimestampContainer[timestampId].setEndTimestamp(endTimestamp);
    
}

uint64_t RoundTripMeasurement::getTimestampDifference(uint64_t timestampId){
    return this->messageTimestampContainer[timestampId].diffTimestamp;
}

void RoundTripMeasurement::clearTimestamps(){
    this->messageTimestampContainer.clear();
}

RoundTripMeasurement roundTripMeasurement;