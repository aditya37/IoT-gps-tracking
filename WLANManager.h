#ifndef WLANManager_H
#define WLANManager_h
#include <Arduino.h>
#include "LCDBoilerplate.h"

class WLANManager {
  private:
    LCDBoilerplate lb;
    uint8_t lcd_Addr;
    uint8_t lcd_cols;
    uint8_t lcd_rows;
  public:
    WLANManager(LCDBoilerplate lb);
    bool GetWlanStatus();
    void ConnectToWlan(const char *wlanSSID, const char *wlanPassword);
    bool WlanConnectionTicker(int count, int countFailedToconnect);
    void AccessPointMode(String wlanName);
};
#endif
