#include "TransactionState.h"



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

void TransactionState::createTotalJsonMessage()
{
    JsonDocument doc;

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
    Serial.println(jsonMessageBuffer);
}

TransactionState transactionState;