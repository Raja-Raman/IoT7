// otaHelper.h
#ifndef OTAHELPER_H
#define OTAHELPER_H

#include "common.h"
#include "config.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

class OtaHelper {
 public:
    OtaHelper();
    void init(const char* firmware_url);
    void init(const char* firmware_url, const char* version_url);
    void check_and_update();
    bool check_version();    
    void update_firmware();
 private:
    bool check_verison_enabled = false;
    char versionUrl[MAX_NAME_LENGTH];
    char firmwareUrl[MAX_NAME_LENGTH];    
};

#endif 
