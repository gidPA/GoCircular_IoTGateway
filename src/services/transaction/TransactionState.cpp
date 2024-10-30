#include "TransactionState.h"
#include "services/mqtt/mqttServices.h"
#include "services/config/rvmConfig.h"
#include "services/message/messageExchangeObj.h"
#include "services/time/timeHandler.h"
#include "services/transaction/memberModeConfirm.h"
#include "services/analytics/RoundTripMeasurement.h"


void TransactionState::setTransactionAsMemberMode(char* memberID){
    if(!isBusy){
        int response = requestConfirmationToRVM();
        char readiness[6];
        snprintf(
            readiness, 
            sizeof(readiness),
            "%d",
            response
        );
        if (response == 1){ // RVM Controller Approves Member Mode
            strncpy(this->memberID, memberID, sizeof(this->memberID));
            this->isMemberMode = true;
            this->isBusy = true;
        }
        mqttClient.publish(rvmConfig.setMemberModeResponseTopic, readiness);
    }
    else{
        mqttClient.publish(rvmConfig.setMemberModeResponseTopic, "127");
    }
}


void TransactionState::resetTransaction()
{
    
    isBusy = false;
    isMemberMode = false;
    currentPoints = 0;
    itemCount = 0;
    memset(items, 0, sizeof(items));
    strncpy(jsonMessageBuffer, "", MAX_TRANSACTION_JSON_BUFFER_SIZE);
}

void TransactionState::appendNewItem(uint64_t enteredItem[], byte itemDataLength)
{
    //Serial.printf("[TRANSACTION STATE] Entered Item: %d %d %d", items[itemCount][0], items[itemCount][1], items[itemCount][2]);

    memcpy(items[itemCount], enteredItem, itemDataLength);

    //Serial.printf("[TRANSACTION STATE] Entered Item in memory: %d %d %d", items[itemCount][0], items[itemCount][1], items[itemCount][2]);

    itemCount += 1;
    currentPoints += enteredItem[2];
}

void TransactionState::finalizeTransaction(char* transactionReportTopic){
    Serial.println("[TRANSACTION_STATE] Finalizing Transaction....");
    createTotalJsonMessage();

    for(int i = 0; i < 3; i++){
        bool isSuccesful = mqttClient.publish(transactionReportTopic, jsonMessageBuffer);
        if(isSuccesful){
            Serial.println("[TRANSACTION_STATE] Transaction successful!");
            break;
        } else if(i == 2){
            Serial.println("[TRANSACTION_STATE] Failed to report transaction");
        }
    }


    resetTransaction();
    roundTripMeasurement.clearTimestamps();
}

void TransactionState::createTotalJsonMessage()
{
    JsonDocument doc;

    char timeStringBuffer[30];
    createSQLTimestamp(&timeObj, timeStringBuffer, sizeof(timeStringBuffer));

    
    // doc["isSuccessful"] = true;
    doc["transactionDate"] = timeStringBuffer;
    doc["userID"] = memberID;
    doc["rvmID"] = rvmConfig.id;
    // doc["totalPoints"] = currentPoints;

    JsonArray recyclableData = doc["recyclableItems"].to<JsonArray>();

    for (int i = 0; i < itemCount; i++)
    {
        JsonArray recyclableItem = recyclableData.add<JsonArray>();
        recyclableItem.add(items[i][0]);
        recyclableItem.add(items[i][1]);
        recyclableItem.add(items[i][2]);
    }
    if(isMemberMode){
        JsonArray latencyData = doc["latencyData"].to<JsonArray>();
        roundTripMeasurement.createTimestampJsonArray(latencyData);
    }


    doc.shrinkToFit(); // optional

    serializeJson(doc, jsonMessageBuffer);


}

void TransactionState::previewTotalJsonMessage()
{
    createTotalJsonMessage();
    Serial.printf("[TRANSACTION_STATE] JSON message Preview: \n%s\n[TRANSACTION_STATE] End of Preview\n",jsonMessageBuffer);
}

void TransactionState::initializeTransaction(){
    getCurrentTimeObject(&timeObj);
    this->isBusy = true;
    Serial.print("[TRANSACTION_STATE] New transaction iniitialized on:");
    Serial.println(&timeObj, "%Y-%m-%d %H:%M:%S");
}

TransactionState transactionState;