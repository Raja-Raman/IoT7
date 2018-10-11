// flashHelper.cpp

/*************************************************************************************** 
   Do NOT edit this file unless absolutely required !!!
   If edited, it requires matching changes in all the dependent projects which use OTA. 
****************************************************************************************/

#include "common.h"
#include "flashHelper.h" 

void FlashHelper::begin() {
    SERIAL_PRINT("Beginning EEPROM with ");
    SERIAL_PRINT(TOTAL_ALLOC_SIZE);
    SERIAL_PRINTLN(" bytes of memory..");    
    EEPROM.begin(TOTAL_ALLOC_SIZE);  // request memory allocation for all 3 copies
}

void FlashHelper::commit() {
    EEPROM.commit(); 
    SERIAL_PRINTLN ("EEPROM writes committed.");
}

void FlashHelper::end() {
    EEPROM.end(); 
    SERIAL_PRINTLN ("EEPROM released.");
}

/* Write 3 copies of the data structure in flash memory */
bool FlashHelper::writeBlocks (DeviceIdentifier ident) {    
    StorageBlock block;
    block.magic_number = MAGIC_NUMBER;  // finger print of the data block
    block.device_id = ident.device_id;
    block.group_id =  ident.group_id;
    block.reserved = RESERVED;   // for now, use this as an additional magic number
    
    if (strlen(ident.firmware_server_URL) > (MAX_STRING_SIZE-1)) {
        SERIAL_PRINT("Firmware server URL length: ");
        SERIAL_PRINTLN (strlen(ident.firmware_server_URL)); 
        SERIAL_PRINT("The firmware server URL is too long ! Maximum allowed is ");
        SERIAL_PRINT (MAX_STRING_SIZE-1);
        SERIAL_PRINTLN(" bytes");
        return false;
    }
    strncpy (block.firmware_server_URL, ident.firmware_server_URL, MAX_STRING_SIZE);
    int structSize = sizeof(block);
    SERIAL_PRINT ("Size of StorageBlock structure: ");
    SERIAL_PRINT(structSize);     
    SERIAL_PRINTLN(" bytes");
    if (structSize > (BLOCK_ADDRESS2 - BLOCK_ADDRESS1) || 
        structSize > (BLOCK_ADDRESS3 - BLOCK_ADDRESS2) || 
        structSize > (TOTAL_ALLOC_SIZE - BLOCK_ADDRESS3)) {
        SERIAL_PRINTLN("The StorageBlock structure is too big to fit in the allocated memory. Aborting...");
        return false;
    }
    begin();  
    SERIAL_PRINTLN("Writing device id...");
 
    EEPROM.put (BLOCK_ADDRESS1, block);
    EEPROM.put (BLOCK_ADDRESS2, block);
    EEPROM.put (BLOCK_ADDRESS3, block);
    
    commit();  // do not end, as you will need to read it back later  
    delay(500);
        
    SERIAL_PRINTLN ("\nFinished writing.");
    return true;
}  

/* Read all 3 blocks and compare them. If OK, return one copy of device identifier 
   Return value of the function : none
   But it returns a DeviceIdentifier, through the reference parameter. NOTE: If the DeviceIdentifier  
   has is_corrupted flag set, all other fields are invaild. Do not use any of them.
*/
void FlashHelper::readBlocks (DeviceIdentifier& ident) {
    DeviceIdentifier ident1, ident2, ident3;  
    if (readBlock(BLOCK_ADDRESS1, ident1)) {  // true if the block is corrupted
        SERIAL_PRINTLN ("Block 1 is corrupted");      
        ident.is_corrupted = true;
        return;
    }
    if (readBlock(BLOCK_ADDRESS2, ident2)) { // true if the block is corrupted
        SERIAL_PRINTLN ("Block 2 is corrupted");      
        ident.is_corrupted = true;      
        return;    
    }
    if (readBlock(BLOCK_ADDRESS3, ident3)) { // true if the block is corrupted
        SERIAL_PRINTLN ("Block 3 is corrupted");
        ident.is_corrupted = true;      
        return; 
    }
    if (ident1.device_id != ident2.device_id) {
        SERIAL_PRINTLN ("Device_id in copies 1&2 do not match !");
        ident.is_corrupted = true;
        return;
    }
    if (ident1.device_id != ident3.device_id) {
        SERIAL_PRINTLN ("Device_id in copies 1&3 do not match !");
        ident.is_corrupted = true;      
        return; 
    }    
    if (ident1.group_id != ident2.group_id) {
        SERIAL_PRINTLN ("Group_id in copies 1&2 do not match !");
        ident.is_corrupted = true;      
        return; 
    }
    if (ident1.group_id != ident3.group_id) {
        SERIAL_PRINTLN ("Group_id in copies 1&3 do not match !");
        ident.is_corrupted = true;      
        return; 
    }
    ident.is_corrupted = false;
    ident.device_id = ident1.device_id;
    ident.group_id = ident1.group_id;    
    strncpy (ident.firmware_server_URL, ident1.firmware_server_URL, MAX_STRING_SIZE);         
}

/* Returns true if the block is CORRUPTED, and false if the data is good */
bool FlashHelper::readBlock (int block_addr, DeviceIdentifier& ident) {
    StorageBlock block;  
    EEPROM.get (block_addr, block);
#ifdef VERBOSE    
    SERIAL_PRINT("address :");
    SERIAL_PRINTLN(block_addr);    
    SERIAL_PRINT("magic_number: ");
    SERIAL_PRINTLNF(block.magic_number, HEX);   
    SERIAL_PRINT("device_id: ");
    SERIAL_PRINTLN(block.device_id);   
    SERIAL_PRINT("group_id: ");
    SERIAL_PRINTLN(block.group_id);   
    SERIAL_PRINT("reserved: ");
    SERIAL_PRINTLNF(block.reserved, HEX); 
    SERIAL_PRINT("Firmware server URL: ");
    SERIAL_PRINTLN(block.firmware_server_URL);     
    SERIAL_PRINTLN("-------------------------");   
#endif    
    ident.is_corrupted = false;    
    if (block.magic_number != MAGIC_NUMBER) {
        ident.is_corrupted = true;
        SERIAL_PRINTLN("ERROR: Magic number is corrupted !");
    }
    ident.device_id = block.device_id;
    ident.group_id = block.group_id;
    if (strlen(block.firmware_server_URL) > (MAX_STRING_SIZE-1)) {
        SERIAL_PRINT("ERROR: The firmware server URL is too long !");
        ident.is_corrupted = true;
    }    
    strncpy (ident.firmware_server_URL, block.firmware_server_URL, MAX_STRING_SIZE);    
    return(ident.is_corrupted ? true : false);
}

bool FlashHelper::testMemory() {
    SERIAL_PRINTLN("--------------------------");
    SERIAL_PRINTLN ("Performing memory self-test...");
    //SERIAL_PRINT ("\nsize of long: ");  // 4 bytes
    //SERIAL_PRINTLN (sizeof(long));
    SERIAL_PRINT ("size of StorageBlock: ");
    SERIAL_PRINTLN (sizeof(StorageBlock)); 
      
    uint32_t realSize = ESP.getFlashChipRealSize();
    uint32_t ideSize = ESP.getFlashChipSize();  // set in the IDE
    FlashMode_t ideMode = ESP.getFlashChipMode();

    SERIAL_PRINTF("Flash chip id:   %08X\n", ESP.getFlashChipId());
    SERIAL_PRINTF("Flash real size: %u\n", realSize);
    SERIAL_PRINTF("Flash IDE  size: %u\n", ideSize);
    SERIAL_PRINTF("Flash IDE speed: %u\n", ESP.getFlashChipSpeed());
    SERIAL_PRINTF("Flash IDE mode:  %s\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? 
        "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));

    boolean result = 1;
    if(ideSize != realSize) {
        SERIAL_PRINTLN("Flash Chip configuration in the IDE is wrong!\n");
        result = 0;
    } else {
        SERIAL_PRINTLN("Flash Chip configuration is OK.");
        result = 1;
    }
    SERIAL_PRINTLN("--------------------------");
    return (result);
}



 