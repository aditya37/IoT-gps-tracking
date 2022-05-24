#ifndef PUBSUBManager_H
#define PUBSUBManager_H
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "LCDBoilerplate.h"
#include "WLANManager.h"

class PUBSUBManager {
  private:
    WiFiClient espclient;
    PubSubClient pubsub;
    WLANManager wlm;
    LCDBoilerplate lb;
    uint8_t lcd_Addr;
    uint8_t lcd_cols;
    uint8_t lcd_rows;
    const char *mqtt_broker;
    const char *mqtt_username;
    const char *mqtt_password;
    int mqtt_port;
    char RegisterDevicePayload[1024];
  public:
    PUBSUBManager(const char *mqtt_broker, const char *mqtt_username, const char *mqtt_password, int mqtt_port, WLANManager wlm, LCDBoilerplate lb);
    bool ConnectingToBroker(String device_id);
    void Loop();
    void MessageCallback(std::function<void(char*, uint8_t*, unsigned int)> callback);
    bool Publish(const char* topic, const char* payload);
    boolean Subscribe(const char* topic, uint8_t qos);
    // ==========================================================
    // for device management in backend
    // ==========================================================
    bool RequestRegisterDevice(const char* topic, String device_id, unsigned long timestamp);
};

#endif
