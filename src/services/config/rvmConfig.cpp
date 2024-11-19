#include "rvmConfig.h"
#include "rvmCred.h"
#include <Arduino.h>

RVMConfig rvmConfig;

void RVMConfig::initiate() {

    snprintf(itemPendingTopic, sizeof(itemPendingTopic), "%s/%s/%s", mqttTopicPrefix, id, itemPendingSubtopic);
    snprintf(itemEntryTopic, sizeof(itemEntryTopic), "%s/%s/%s", mqttTopicPrefix, id, itemEntrySubtopic);
    snprintf(itemRejectTopic, sizeof(itemRejectTopic), "%s/%s/%s", mqttTopicPrefix, id, itemRejectSubtopic);
    snprintf(transactionReportTopic, sizeof(transactionReportTopic), "%s/%s/%s", mqttTopicPrefix, id, transactionReportSubtopic);
    snprintf(rvmStatusTopic, sizeof(rvmStatusTopic), "%s/%s/%s", mqttTopicPrefix, id, rvmStatusSubtopic);
    // snprintf(binFullAlertTopic, sizeof(binFullAlertTopic), "%s/%s/%s", mqttTopicPrefix, id, binFullAlertSubtopic);
    // snprintf(coinEmptyTopic, sizeof(coinEmptyTopic), "%s/%s/%s", mqttTopicPrefix, id, coinEmptySubtopic);
    
    snprintf(setMemberModeResponseTopic, sizeof(setMemberModeResponseTopic), "%s/%s/%s", mqttTopicPrefix, id, setMemberModeResponseSubtopic);
    snprintf(setMemberModeTopic, sizeof(setMemberModeTopic), "%s/%s/%s", mqttTopicPrefix, id, setMemberModeSubtopic);
    snprintf(setExchangeRateTopic, sizeof(setExchangeRateTopic), "%s/%s/%s", mqttTopicPrefix, id, setExchangeRateSubtopic);

    snprintf(timestampMarkingTopic, sizeof(timestampMarkingTopic), "%s/%s/%s", mqttTopicPrefix, id, timestampMarkingSubtopic);

    snprintf(httpAuthURL, sizeof(httpAuthURL), "%s/api/rvm/auth", httpAddress);


    Serial.println("");
    Serial.printf("[RVMConfig] itemPendingTopic: %s\n", itemPendingTopic);
    Serial.printf("[RVMConfig] itemEntryTopic: %s\n", itemEntryTopic);
    Serial.printf("[RVMConfig] transactionReportTopic: %s\n", transactionReportTopic);
    Serial.printf("[RVMConfig] rvmStatusTopic: %s\n", rvmStatusTopic);
    // Serial.printf("[RVMConfig] binFullAlertTopic: %s\n", binFullAlertTopic);
    // Serial.printf("[RVMConfig] coinEmptyTopic: %s\n", coinEmptyTopic);
    Serial.printf("[RVMConfig] setMemberModeTopic: %s\n", setMemberModeTopic);
    Serial.printf("[RVMConfig] setMemberModeResponseTopic: %s\n", setMemberModeResponseTopic);
    Serial.printf("[RVMConfig] setExchangeRateTopic: %s\n", setExchangeRateTopic);
    Serial.printf("[RVMConfig] HTTP Authentication URL: %s\n", httpAuthURL);
}

void RVMConfig::getMQTTUrl(char mqtturl[]){
    char buffer[maxSize];

    snprintf(buffer, maxSize, "%s://%s", mqttprefix, mqttAddress);
    strncpy(mqtturl, buffer, maxSize);
}
