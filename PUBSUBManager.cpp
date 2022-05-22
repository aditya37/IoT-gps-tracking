#include "PUBSUBManager.h"
#include <ArduinoJson.h>

PUBSUBManager::PUBSUBManager(const char *mqtt_broker, const char *mqtt_username, const char *mqtt_password, int mqtt_port): pubsub(espclient) {
  this->mqtt_broker = mqtt_broker;
  this->mqtt_username = mqtt_username;
  this->mqtt_password = mqtt_password;
  this->mqtt_port = mqtt_port;
  pubsub.setServer(mqtt_broker, mqtt_port);
}

bool PUBSUBManager::ConnectingToBroker(String device_id) {
  while (!pubsub.connected()) {
    if (pubsub.connect(device_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Public emqx mqtt broker connected");
      return true;
    } else {
      Serial.print("failed with state ");
      Serial.print(pubsub.state());
      return false;
    }
  }
}

//MessageCallback....
void _callback(char *topic, byte *payload, unsigned int length) {
  Serial.println(topic);
}
void PUBSUBManager::MessageCallback() {
  pubsub.setCallback(_callback);
}
//Publish....
bool PUBSUBManager::Publish(const char* topic, const char* payload) {
  bool isPublished = pubsub.publish(topic, payload);
  if (!isPublished) {
    return false;
  } else {
    return true;
  }
}
//Loop...
void PUBSUBManager::Loop() {
  pubsub.loop();
}

//Hertbeat...
void PUBSUBManager::Heartbeat(int duration, const char* topic, String deviceid) {
  unsigned long current_milis = millis();
  if (current_milis - last_milis >= duration) {
    last_milis = current_milis;
    // create json
    StaticJsonDocument<256> doc;
    doc["device_id"] = deviceid;
    doc["status"] = "HEALTH";
    serializeJson(doc, HeartbeatMessage);

    bool isPublished = Publish(topic, HeartbeatMessage);
    if (!isPublished) {
      Serial.println("Device is off");
      return;
    }
  }
}
