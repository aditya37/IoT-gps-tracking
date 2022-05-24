#ifndef WLANManager_H
#define WLANManager_H
#include <Arduino.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "LCDBoilerplate.h"

class WLANManager {
  private:
    LCDBoilerplate lb;
    WiFiUDP ntpUDP;
    NTPClient timeClient;
    uint8_t lcd_Addr;
    uint8_t lcd_cols;
    uint8_t lcd_rows;
    const char*   _poolServerName; // Default time server
    unsigned long epoch_time;
    long NTP_OFFSET; // GMT+7
  public:
    WLANManager(LCDBoilerplate lb);
    bool GetWlanStatus();
    void ConnectToWlan(const char *wlanSSID, const char *wlanPassword);
    bool WlanConnectionTicker(int count, int countFailedToconnect);
    void AccessPointMode(String wlanName);
    int32_t GetWlanSignalStrength();
    String GetMacAddress();
    // NTP
    void NTPBegin();
    void UpdateNTP();
    unsigned long GetEpochTime();
    
};
#endif
