//Modified
// commandHandler.h

#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "common.h"
#include "config.h"
#include "otaHelper.h"
#include "flashHelper.h"

class CommandHandler {
public:
    CommandHandler();
    void init (Config* configptr);
    void disable();
    void handleCommand (const CommandData& cmd);   
private:
    Config *pC;
    //OtaHelper O;  // TODO: move this to the stack?
    CommandData command_cache;  
    void copyCommand (const CommandData& cmd) ;   
    void sendHandshake(); 
    void setDeviceIDandGroupID();
    bool verifyURL();
    bool checkRecipientAddress (const CommandData& cmd); 
    void setFirmwareServerURL(); 
    void setFlashConfig(); 
    void writeFlashConfig();
    void sendParsingError (); 
    void updateFirmware();
    void printCommand();
    void restart ();
  
};

#endif 
