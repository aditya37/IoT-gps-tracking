#include "WLANManager.h"
#include "LCDBoilerplate.h"
#include <ESP8266WiFi.h>

// Constructor
WLANManager::WLANManager(LCDBoilerplate lb): lb(lcd_Addr, lcd_cols, lcd_rows) {
  this->lb = lb;
}

// getwlanStatus
bool WLANManager::GetWlanStatus() {
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }
  return true;
}

// ConnectToWlan...
void WLANManager::ConnectToWlan(const char *wlanSSID, const char *wlanPassword) {
  WiFi.begin(wlanSSID, wlanPassword);
}

//WlanConnectionTicker...
bool WLANManager::WlanConnectionTicker(int count, int countFailedToconnect) {
  for (int n = 0; n <= count; n++) {
    // TODO: blink lamp until connect to WLAN
    if (!GetWlanStatus()) {
      lb.WriteConnectingWlan();
      if (n >= countFailedToconnect) {
        // TODO: Write to lcd if failed to connect wlan
        lb.WriteFailedConnectWifi();
        // TODO: Serve http
        return false;
      }
    } else {
      //TODO: Write to lcd if wlan success conect
      lb.Clear();
      return true;
    }
    // TODO: TURN OFF Lamp
    delay(1000);
  }
}

// AccessPointMode...
void WLANManager::AccessPointMode(String wlanName) {

  // Set mode to access point
  WiFi.mode(WIFI_STA);
  //  Disconnect if wlan connected with another WLAN
  WiFi.disconnect();
  //  break before scan a network
  delay(1000);

  // get count neraby network
  int countNearbyNet = WiFi.scanNetworks();
  if (countNearbyNet == 0) {
    Serial.println("Nearby network not found");
  } else {
    Serial.print("Count Nearby Network " + countNearbyNet);
    // loop for show detail nearby network
    for (int n = 0; n < countNearbyNet; n ++) {
      Serial.println("");
      // TODO: Convert to html string and show to web
      Serial.println(WiFi.SSID(n));
    }
  }
  WiFi.softAP(wlanName, "");
}
