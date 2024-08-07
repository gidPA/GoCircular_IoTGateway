#include "TransactionState.h"
#include "services/mqtt/mqttServices.h"
#include "rvmConfig.h"

void TransactionState::setTransactionAsMemberMode(int memberID){
    if(!isBusy){
        this->memberID = memberID;
    }
}


void TransactionState::resetTransaction()
{
    isBusy = true;
    isMemberMode = false;
    currentPoints = 0;
    itemCount = 0;
    memset(items, 0, sizeof(items));
    strncpy(jsonMessageBuffer, "", MAX_TRANSACTION_JSON_BUFFER_SIZE);
}

void TransactionState::appendNewItem(byte enteredItem[], byte itemDataLength)
{
    //Serial.printf("[TRANSACTION STATE] Entered Item: %d %d %d", items[itemCount][0], items[itemCount][1], items[itemCount][2]);

    memcpy(items[itemCount], enteredItem, itemDataLength);

    //Serial.printf("[TRANSACTION STATE] Entered Item in memory: %d %d %d", items[itemCount][0], items[itemCount][1], items[itemCount][2]);

    itemCount += 1;
    currentPoints += enteredItem[2];
}

void TransactionState::finalizeTransaction(){
    Serial.println("[TRANSACTION_STATE] Finalizing Transaction....");
    createTotalJsonMessage();
    char topic[50];
    snprintf(topic, 50, "%s/transaction/report", rvmConfig.mqttTopicHead);

    for(int i = 0; i < 3; i++){
        bool isSuccesful = mqttClient.publish(topic, jsonMessageBuffer);
        if(isSuccesful){
            Serial.println("[TRANSACTION_STATE] Transaction successful!");
            break;
        } else if(i == 2){
            Serial.println("[TRANSACTION_STATE] Failed to report transaction");
        }
    }


    resetTransaction();
}

void TransactionState::createTotalJsonMessage()
{
    JsonDocument doc;

    doc["isMemberMode"] = isMemberMode;
    doc["userID"] = memberID;
    doc["totalPoints"] = currentPoints;

    JsonArray recyclableData = doc["recyclableItems"].to<JsonArray>();

    for (int i = 0; i < itemCount; i++)
    {
        JsonArray recyclableItem = recyclableData.add<JsonArray>();
        recyclableItem.add(items[i][0]);
        recyclableItem.add(items[i][1]);
        recyclableItem.add(items[i][2]);
    }

    doc.shrinkToFit(); // optional

    serializeJson(doc, jsonMessageBuffer);
}

void TransactionState::previewTotalJsonMessage()
{
    createTotalJsonMessage();
    Serial.printf("[TRANSACTION_STATE] JSON message Preview: \n%s\n[TRANSACTION_STATE] End of Preview\n",jsonMessageBuffer);
}

TransactionState transactionState;