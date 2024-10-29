#ifndef TRANSACTION_STATE_H
#define TRANSACTION_STATE_H

#include <Arduino.h>
#include <string.h>
#include <ArduinoJson.h>
#include <time.h>

#define MAX_TRANSACTION_JSON_BUFFER_SIZE 1024



struct TransactionState
{
    tm timeObj;
    bool isBusy = false;
    bool isMemberMode = false;
    char memberID[15] = "1000";

    uint64_t items[20][4];
    int currentPoints = 0;
    int itemCount = 0;

    char jsonMessageBuffer[MAX_TRANSACTION_JSON_BUFFER_SIZE];

    void setTransactionAsMemberMode(char* memberID);
    void initializeTransaction();

    void finalizeTransaction(char* transactionReportTopic);
    void resetTransaction();

    void appendNewItem(uint64_t enteredItem[], byte itemDataLength);
    
    void createTotalJsonMessage();


    void previewTotalJsonMessage();
};

extern TransactionState transactionState;

#endif // TRANSACTION_STATE_H