#ifndef PUBSUBManager_H
#define PUBSUBManager_H
#include <ESP8266WiFi.h>
#include <PubSubClient.h>


class PUBSUBManager {
  private:
    WiFiClient espclient;
    PubSubClient pubsub;
    const char *mqtt_broker;
    const char *mqtt_username;
    const char *mqtt_password;
    int mqtt_port;
    int last_milis = 0;
    char HeartbeatMessage[128];
  public:
    PUBSUBManager(const char *mqtt_broker, const char *mqtt_username, const char *mqtt_password, int mqtt_port);
    bool ConnectingToBroker(String device_id);
    void Loop();
    void MessageCallback();
    bool Publish(const char* topic, const char* payload);
    void Heartbeat(int duration, const char* topic, String deviceid);
};

#endif
