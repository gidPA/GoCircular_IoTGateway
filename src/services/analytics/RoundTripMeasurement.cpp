#include <Arduino.h>
#include "RoundTripMeasurement.h"
#include <ArduinoJson.h>

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

void RoundTripMeasurement::createTimestampJsonArray(JsonArray &arr){
    // for(auto& [id, ts] : this->messageTimestampContainer){
    //     arr.add(ts.diffTimestamp);
    // }

    for (auto it = this->messageTimestampContainer.begin(); it != this->messageTimestampContainer.end(); ++it) {
        arr.add(it->second.diffTimestamp);
    }
}

RoundTripMeasurement roundTripMeasurement;