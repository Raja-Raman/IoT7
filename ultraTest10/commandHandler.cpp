// commandHandler.cpp

#include "commandHandler.h"

//extern void send_data();
extern void send_on_demand_data();
void send_acknowledgement(long command, int response);

    const char* cmd_names[] = { // maintain the same order as in common.h !
    "CMD_HELLO",
    "CMD_GET_DATA",
    "CMD_SET_FLOOR",
    "CMD_SET_THRESHOLD",
    "CMD_SET_DATA_INTERVAL",
    "CMD_SET_WAKE_DURATION",
    "CMD_SET_SLEEP_DURATION",
    "CMD_SET_ID_GROUP",
    "CMD_SET_FW_SERVER_URL",
    "CMD_UPDATE_FIRMWARE",
    "CMD_GET_FLASH_CONFIG",
    "CMD_SET_FLASH_CONFIG",
    "CMD_WRITE_FLASH_CONFIG",
    "CMD_RESTART",
    "CMDX_PARSING_ERROR"
    };  
    
CommandHandler::CommandHandler() {
}
 
void CommandHandler::init (Config* configptr) {
    this->pC = configptr;
} 

void CommandHandler::disable() {
    //SERIAL_PRINTLN ("\Disabling Command Handler..");
    SERIAL_PRINTLN("Command Handler.disable not implemented.");
    // TODO: unsubscribe from mqtt ?
}

//  Makes a local copy of the command object before another mqtt command arrives
// TODO: keep a small queue of command objects, at least 2.
void CommandHandler::copyCommand (const CommandData& cmd) {
    command_cache.device_id = cmd.device_id;
    command_cache.group_id = cmd.group_id;
    command_cache.command = cmd.command;
    command_cache.long_param = cmd.long_param;
    if (strlen(cmd.string_param) > 0) {
        strncpy (command_cache.string_param, cmd.string_param, MAX_NAME_LENGTH-1);
        command_cache.string_param[MAX_NAME_LENGTH-1] = '\0';   
    }                
}

bool CommandHandler::checkRecipientAddress (const CommandData& cmd) {
    if (cmd.device_id != pC->device_id && cmd.device_id != BROADCAST_DEVICE_ID) 
        return (false);
    if (cmd.group_id != pC->group_id && cmd.group_id != BROADCAST_GROUP_ID) 
        return (false);
    return (true); 
}

void CommandHandler::handleCommand (const CommandData& cmd) {
    if (cmd.command == CMDX_PARSING_ERROR) {
        sendParsingError();
        return;
    }
    if (!checkRecipientAddress(cmd))
        return;
        
    // make a backup copy first
    copyCommand (cmd);
    // at this point, the copy has all the data, cmd parameter is safe to be released
    
    #ifdef VERBOSE_MODE
        printCommand();
    #endif

    switch(command_cache.command) {
        case CMD_HELLO:
            sendHandshake();
            break;      
        case CMD_SET_FW_SERVER_URL:
            setFirmwareServerURL();
            break;
        case CMD_UPDATE_FIRMWARE:
            updateFirmware();
            break;      
        case CMD_RESTART:
            restart();
            break;           
        case CMD_GET_DATA:
            send_on_demand_data();
            break;  
        case CMD_SET_ID_GROUP:  
;           setDeviceIDandGroupID ();
            break;                  
        case CMD_SET_FLASH_CONFIG: // this only sets them temporarily for the session
            setFlashConfig();
            break;  
        case CMD_WRITE_FLASH_CONFIG: // to save it permanently, you must call this 
            writeFlashConfig();
            break;                          
        default:
            SERIAL_PRINT ("Command ");
            //SERIAL_PRINT(cmd_names[command_cache.command]);  //index can be out of range  
            SERIAL_PRINT(command_cache.command);     
            SERIAL_PRINTLN(" not implemented");
            break;
    }
    // TODO: take other actions
}

void CommandHandler::sendHandshake() {
    SERIAL_PRINTLN("Replying to hello...");
    send_acknowledgement(command_cache.command, STATUS_CMD_ACK);    
}

void CommandHandler::sendParsingError () {
    SERIAL_PRINTLN("Command Parsing Error");
    SERIAL_PRINTLN("sendParsingError not implemented");  // TODO
}

// temporarily set device_id and group_id for the rest of the session
// useful for remotely taking control and troubleshoot
void CommandHandler::setDeviceIDandGroupID()
{
    send_acknowledgement(command_cache.command, STATUS_CMD_ACK); // with old id
    SERIAL_PRINTLN("setDeviceIDandGroupID not implemented");
    // TODO: send the new values as a json object in string_param and parse 
    /*
    pC->device_id = command_cache.device_id; // this could only be the old id !
    pC->group_id = command_cache.group_id;
    SERIAL_PRINT("Till this device is restarted, temporary device id is: ");
    SERIAL_PRINTLN(pC->device_id);
    SERIAL_PRINT("Temporary group id is: ");
    SERIAL_PRINTLN(pC->group_id);
    send_acknowledgement(command_cache.command, STATUS_CMD_SUCCESS);  // TODO: again with new id ?
    */
}

bool CommandHandler::verifyURL() {
    SERIAL_PRINTLN ("verifyURL not implemented");
    return (true);  // TODO: check URL length and format [starts with http://]
}

// this is temporary URL setting in pC; it lasts only till a restart
void CommandHandler::setFlashConfig() {
    send_acknowledgement(command_cache.command, STATUS_CMD_ACK); // with old id
    SERIAL_PRINTLN("setFlashConfig not implemented");
    // TODO: send the new values as a json object in string_param and parse    
    /*
    if (verifyURL()) {
        pC->device_id = command_cache.device_id;
        pC->group_id = command_cache.group_id;    
        strncpy (pC->firmware_update_URL, command_cache.string_param, MAX_NAME_LENGTH);
        pC->firmware_update_URL[MAX_NAME_LENGTH-1] = '\0';  // if overflows, unterminated ! 
    } else {
        SERIAL_PRINTLN ("verifyURL failed");
        send_acknowledgement(command_cache.command, STATUS_CMD_FAILED);   
        return;
    }
    SERIAL_PRINT("Till this device is restarted, temporary device_id is: ");
    SERIAL_PRINTLN(pC->device_id);
    SERIAL_PRINT("Temporary group_id is: ");
    SERIAL_PRINTLN(pC->group_id);    
    SERIAL_PRINT("Temporary* firmware update URL: ");
    SERIAL_PRINTLN(pC->firmware_update_URL);
    send_acknowledgement(command_cache.command, STATUS_CMD_SUCCESS);
    */
}

// temporarily set device config for the current session
void CommandHandler::setFirmwareServerURL () {
    send_acknowledgement(command_cache.command, STATUS_CMD_ACK);
    if (verifyURL()) {
    strncpy (pC->firmware_update_URL, command_cache.string_param, MAX_NAME_LENGTH);
    pC->firmware_update_URL[MAX_NAME_LENGTH-1] = '\0';  // if overflows, unterminated ! 
    } else {
        SERIAL_PRINTLN ("verifyURL failed");
        send_acknowledgement(command_cache.command, STATUS_CMD_FAILED);   
        return;
    }
    SERIAL_PRINT("New *temporary* firmware update URL: ");
    SERIAL_PRINTLN(pC->firmware_update_URL);    
    send_acknowledgement(command_cache.command, STATUS_CMD_SUCCESS);    
}

void CommandHandler::updateFirmware(){  
    send_acknowledgement(command_cache.command, STATUS_CMD_ACK);
    OtaHelper O;
    O.init(pC->firmware_update_URL);
    O.check_and_update(); // this will restart the device
}

// permanently save the config to flash
void CommandHandler::writeFlashConfig() {
    send_acknowledgement(command_cache.command, STATUS_CMD_ACK); // with old id
    SERIAL_PRINTLN("writeFlashConfig not implemented");
    /***
    if (verifyURL()) {
        pC->device_id = command_cache.device_id;
        pC->group_id = command_cache.group_id;    
        strncpy (pC->firmware_update_URL, command_cache.string_param, MAX_NAME_LENGTH);
        pC->firmware_update_URL[MAX_NAME_LENGTH-1] = '\0';  // if overflows, unterminated ! 
    } else {
        SERIAL_PRINTLN ("verifyURL failed");
        send_acknowledgement(command_cache.command, STATUS_CMD_FAILED);   
        return;
    }
    SERIAL_PRINT("device_id will be permanently changed to: ");
    SERIAL_PRINTLN(pC->device_id);
    SERIAL_PRINT("group_id will be permanently changed to: ");
    SERIAL_PRINTLN(pC->group_id);    
    SERIAL_PRINT("firmware update URL will be permanently changed to: ");
    SERIAL_PRINTLN(pC->firmware_update_URL);    
    DeviceIdentifier ident;
    ident.device_id = pC->device_id;
    ident.group_id = pC->group_id;
    strncpy (ident.firmware_server_URL, pC->firmware_update_URL, MAX_NAME_LENGTH);
    ident.firmware_server_URL[MAX_NAME_LENGTH-1] = '\0';     
    FlashHelper F;
    F.begin();
    F.writeBlocks(ident);
    F.end();
    send_acknowledgement(command_cache.command, STATUS_CMD_SUCCESS); 
    ***/
}

void CommandHandler::printCommand() {   // print from cache
    SERIAL_PRINT("device_id: ");
    SERIAL_PRINTLN(command_cache.device_id);
    SERIAL_PRINT("group_id: ");
    SERIAL_PRINTLN(command_cache.group_id);
    SERIAL_PRINT("command: (");
    SERIAL_PRINT(command_cache.command);
    SERIAL_PRINT(") ");
    SERIAL_PRINTLN(cmd_names[command_cache.command]);
    SERIAL_PRINT("long_param: ");
    SERIAL_PRINTLN(command_cache.long_param);
    SERIAL_PRINT("string_param: ");
    SERIAL_PRINTLN(command_cache.string_param);
    SERIAL_PRINTLN("-------------------------");   
}

void CommandHandler::restart () {
    send_acknowledgement(command_cache.command, STATUS_CMD_ACK);
    SERIAL_PRINTLN ("\n\n!!! Parking sensor is about to restart !!!\n");
    delay(1000);
    ESP.restart();

}
