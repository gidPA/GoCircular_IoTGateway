#include "memberModeConfirm.h"
#include "services/message/messageExchangeObj.h"

int requestConfirmationToRVM(){
    messageExchange.createNewMessage(SET_MEMBER_MODE);
    messageExchange.previewMessage();

    int status = messageExchange.sendMessageAndWait();

    if (status == 100){
        return status;
    } else {
        return messageExchange.getMemberModeReadiness();
    }
}