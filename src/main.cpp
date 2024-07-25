#include <Arduino.h>
#include <MessageExchange.h>

MessageExchange messageExchange;


void handleIncomingMessage();


void setup(){
  Serial.begin(115200);
  Serial2.begin(115200);

  messageExchange.setUartDevice(&Serial2);
  messageExchange.setUartMonitoringDevice(&Serial);
  messageExchange.setSignature("MessageExchange");
}

void loop(){
  if(messageExchange.checkMessageEntry()){
    handleIncomingMessage();
  }
}

void handleIncomingMessage(){
  byte messageTopic = messageExchange.handleIncomingMessage();
  messageExchange.previewMessage();
}