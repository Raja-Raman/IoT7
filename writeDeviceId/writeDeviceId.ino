/*
   Stores the device ID, group ID and firmware server URL in EEPROM.
   Use it in conjunction with parking sensor and occupancy sensor programs.
   Increment the device id for each new device and burn it in. Then separately burn the
   parking/occupancy sensor code. The device id will stay safe between power cycles.
   We write 3 copies of the device id to guard against data corruption and 
   compare them during boot up time of the parking/occupancy sensor.
*/

// NOTE: you must increment the device ID in device.h for every new device
// Note: choose the correct ESP board, 4 MB memory and 3 MB SPIFFs in the IDE.

#include "device.h"
#include "common.h"
#include "flashHelper.h"

FlashHelper F;
  
void setup() {
    Serial.begin (115200);
    SERIAL_PRINTLN ("\n\nEEPROM device id writer starting...");
    SERIAL_PRINTLN ("\nDid you increment the device ID in the file device.h ?");
    SERIAL_PRINTLN ("If not, you must do so before proceeding !\n");
    SERIAL_PRINT ("Device ID: ");
    SERIAL_PRINTLN (DEVICE_ID);    // from the file DEVICE.H
    SERIAL_PRINT ("Group ID: ");
    SERIAL_PRINTLN (GROUP_ID);
    SERIAL_PRINT ("Firmware Server URL: ");
    SERIAL_PRINTLN (FIRMWARE_SERVER_URL);
          
    if (!F.testMemory()) {
        SERIAL_PRINTLN ("Basic Memory Test failed !!! aborting.");
        return;
    }
    DeviceIdentifier ident; 
    prepare_device_id (ident);
    if (ident.is_corrupted) {
        SERIAL_PRINTLN ("\nInvalid input data. Aborting.\n");
        return;
    }
    SERIAL_PRINTLN ("\nWriting to flash memory..\n");
    F.writeBlocks(ident);
    yield();
    SERIAL_PRINTLN ("\nReading back memory..\n");
    F.readBlocks(ident);
    F.end();  // do not end if you are going to read it back later in this program

    // FIRST THING you should do is check the is_corrupted flag. If it is set, the rest of
    // the object should be treated as invalid garbage.
    if (ident.is_corrupted) {
        SERIAL_PRINTLN("\nFATAL ERROR: Could not reliably write/read EEPROM. Aborting.");
    } else {
        SERIAL_PRINTLN("\nStored the device id and firmware server URL in EEPROM."); 
        SERIAL_PRINTLN("(This step has to be repeated again only if you erase the EEPROM).");
        SERIAL_PRINTLN("\nNow you can burn the production code into the chip.");   
    }
}

// prepare a device identifier object from globally defined variables
// return value: true if the object is CORRUPTED, false if the object is good.
bool prepare_device_id (DeviceIdentifier & ident) {
    ident.is_corrupted = false; 
    ident.device_id = (long)DEVICE_ID;  // read from device.h file
    ident.group_id  = (long)GROUP_ID;   // read from device.h file
    if (strlen(FIRMWARE_SERVER_URL) > (MAX_STRING_SIZE-1)) {
        SERIAL_PRINT("Firmware server URL length: ");
        SERIAL_PRINTLN (strlen(ident.firmware_server_URL)); 
        SERIAL_PRINT("The firmware server URL is too long ! Maximum allowed is ");
        SERIAL_PRINT (MAX_STRING_SIZE-1);
        SERIAL_PRINTLN(" bytes.");
        ident.is_corrupted = true;
    }    
    strncpy(ident.firmware_server_URL, FIRMWARE_SERVER_URL, MAX_STRING_SIZE);
    return (ident.is_corrupted ? true : false);  // true indicates ERROR
}

void loop() {  
}


