// otaHelper.cpp

#include "otaHelper.h"

OtaHelper::OtaHelper() {
}

/*  If this shows a string size error, shorten your actual web page URL to MAX_NAME_LENGTH bytes. */
void OtaHelper::init(const char* firmware_url) {
    check_verison_enabled = false;
    if (strlen(firmware_url) > MAX_NAME_LENGTH-1) {
        SERIAL_PRINT ("FATAL ERROR: Firmware URL length must be less than ");
        SERIAL_PRINT (MAX_NAME_LENGTH);
        SERIAL_PRINTLN (" bytes");
    }  // proceed anyway !
    strncpy(firmwareUrl, firmware_url, MAX_NAME_LENGTH);
    firmwareUrl[MAX_NAME_LENGTH-1] = '\0';  // if overflows, goes unterminated ! 
}

/*  If this shows a string size error, shorten your actual web page URL to MAX_NAME_LENGTH bytes. */
void OtaHelper::init(const char* firmware_url, const char* version_url){
    check_verison_enabled = true;
    if (strlen(firmware_url) > MAX_NAME_LENGTH-1) {
        SERIAL_PRINT ("FATAL ERROR: Firmware URL length must be less than ");
        SERIAL_PRINT (MAX_NAME_LENGTH);
        SERIAL_PRINTLN (" bytes");
    }  // proceed anyway !
    if (strlen(version_url) > MAX_NAME_LENGTH-1) {
        SERIAL_PRINT ("FATAL ERROR: Version URL length must be less than ");
        SERIAL_PRINT (MAX_NAME_LENGTH);
        SERIAL_PRINTLN (" bytes");
    }  // proceed anyway !        
    strncpy(versionUrl, version_url, MAX_NAME_LENGTH); 
    versionUrl[MAX_NAME_LENGTH-1] = '\0';       // if the url is truncated, it is also left unterminated !
    strncpy(firmwareUrl, firmware_url, MAX_NAME_LENGTH);    
    firmwareUrl[MAX_NAME_LENGTH-1] = '\0';    
}   
    
void OtaHelper::check_and_update() { 
    if (check_version())
        update_firmware();    
}

// TODO: send various status and error messages back to server
bool OtaHelper::check_version() {
    if (!check_verison_enabled) {
        SERIAL_PRINTLN ("Version checking not enabled. Proceeding to update firmware..");
        return (true);
    }
    HTTPClient client;
    SERIAL_PRINTLN("Checking for firmware updates...");
    SERIAL_PRINT("Firmware version file: ");
    SERIAL_PRINTLN(versionUrl);

    boolean result = false;
    client.begin(versionUrl);
    int httpCode = client.GET();
    if(httpCode == 200) {
        String str_version = client.getString();
        SERIAL_PRINT("Current firmware version: ");
        SERIAL_PRINTLN(FIRMWARE_VERSION);
        SERIAL_PRINT("Available firmware version: ");
        SERIAL_PRINTLN(str_version);
        int newVersion = str_version.toInt();
        if(newVersion > FIRMWARE_VERSION) {
            SERIAL_PRINTLN ("New version is available");
            result = true;
        } else { 
            SERIAL_PRINTLN("This device already has the latest version");
            result = false;
        }
    } else { 
        SERIAL_PRINT( "Cannot check firmware version. HTTP error code: " );
        SERIAL_PRINTLN( httpCode );      
        result = false;
    }
    client.end();
    return (result);
}    

// TODO: send various status and error messages back to server 
void OtaHelper::update_firmware() {
    SERIAL_PRINTLN("Updating firmware..");  
    SERIAL_PRINT("Looking for image file: ");
    SERIAL_PRINTLN(firmwareUrl);
    
    ESPhttpUpdate.rebootOnUpdate(REBOOT_AFTER_UPDATE);    
    t_httpUpdate_return ret = ESPhttpUpdate.update(firmwareUrl);

    SERIAL_PRINT("Return code: ");
    SERIAL_PRINTLN(ret);
    switch (ret) {
        case HTTP_UPDATE_OK:
            SERIAL_PRINTLN("HTTP update success !");
            break;
        case HTTP_UPDATE_FAILED:
            SERIAL_PRINT("HTTP update FAILED. Error code = ");
            SERIAL_PRINTLN (ESPhttpUpdate.getLastError()); 
            SERIAL_PRINTLN (ESPhttpUpdate.getLastErrorString().c_str());
            break;
        case HTTP_UPDATE_NO_UPDATES:
            SERIAL_PRINTLN("NO updates available");
            break;
    }  
}
