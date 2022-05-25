#include <ArduinoJson.h>
// custom header
#include "WLANManager.h"
#include "LCDBoilerplate.h"
#include "PUBSUBManager.h"

// PIN
#define LED_WARNING_PIN 0
#define SDAPIN 2 // D2
#define SCLPIN 16 // D1

// Constant
String DEVICE_ID = "iot.1bf7a743-d86a-4709-9832-3f522d4cf645";

/* NOTIFY TOPIC, for notif server or backend
    if device have some event or incident
*/
const char* NOTIFY_TOPIC = "/device/notify/iot.1bf7a743-d86a-4709-9832-3f522d4cf645";
const char* HEARTBEAT_TOPIC = "/device/heartbeat/iot.1bf7a743-d86a-4709-9832-3f522d4cf645";
const char* REQUEST_REGISTER_DEVICE_TOPIC = "/device/req/register/iot.1bf7a743-d86a-4709-9832-3f522d4cf645";

const char *mqtt_broker = "";
const char *mqtt_username = "vps-mqtt";
const char *mqtt_password = "lymousin";
int mqtt_port = 1883;
int HEARTBEAT_DURATION = 10000;
const char *wlanSSID = "KEDAI DJENGGOT $$$";
const char *wlanPassword = "estebuireng";

//Heart beat data
char HeartbeatMessage[1024];
int heartbeat_last_milis = 0;
int minimumSignalRange = -86;


//INSTANCE
LCDBoilerplate lb(0x27, SDAPIN, SCLPIN);
WLANManager wl(lb);
PUBSUBManager pb(mqtt_broker, mqtt_username, mqtt_password, mqtt_port,wl,lb);

//MessageCallback....
void _callback(char *topic, byte *payload, unsigned int length) {
  Serial.println(topic);
}

void Heartbeat(int duration, const char* topic, String deviceid) {
  unsigned long current_milis = millis();
  if (current_milis - heartbeat_last_milis >= duration) {
    heartbeat_last_milis = current_milis;
    // create json
    StaticJsonDocument<1024> doc;
    // check wlan strength
    if (wl.GetWlanSignalStrength() < minimumSignalRange && wl.GetWlanSignalStrength() < -100) {
      Serial.println(wl.GetWlanSignalStrength());
      Serial.println("Sinyal rendah");
      doc["device_id"] = deviceid;
      doc["status"] = "LOW SIGNAL";
      doc["description"] = "Device connected with low signal";
      doc["timestamp"] =  wl.GetEpochTime();;
      serializeJson(doc, HeartbeatMessage);
      bool isPublished = pb.Publish(topic, HeartbeatMessage);
      if (!isPublished) {
        Serial.println("Device is off");
        return;
      }
    } else {
      Serial.println("================================");
      Serial.println(wl.GetWlanSignalStrength());
      Serial.println("Healty Device");
      doc["device_id"] = deviceid;
      doc["status"] = "HEALTHY";
      doc["description"] = "Device is healthy";
      doc["timestamp"] = wl.GetEpochTime();
      serializeJson(doc, HeartbeatMessage);
      bool isPublished = pb.Publish(topic, HeartbeatMessage);
      if (!isPublished) {
        Serial.println("Device is off");
        return;
      }
    }
  }
}

void setup() {

  // Init serial
  Serial.begin(9600);

  // connect to last connected wifi
  wl.ConnectToWlan(wlanSSID, wlanPassword);
  wl.NTPBegin();

  // wlan connection ticker
  bool tickWlanConn = wl.WlanConnectionTicker(40, 20);
  if (!tickWlanConn) {
    wl.AccessPointMode("IoT-Tracking-Device");
  } else {
    bool isConnectedBroker = pb.ConnectingToBroker(DEVICE_ID);
    if (!isConnectedBroker) {
      // write to lcd if not connected
      lb.WriteMQTTState(false);
    };
    // write to lcd if connected to broker
    lb.WriteMQTTState(true);
    // Message callback
    pb.MessageCallback(_callback);
    wl.UpdateNTP();
    
    pb.RequestRegisterDevice(REQUEST_REGISTER_DEVICE_TOPIC, DEVICE_ID,wl.GetEpochTime());
    delay(1000);
    pb.Subscribe(REQUEST_REGISTER_DEVICE_TOPIC,0);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  pb.Loop();
  wl.UpdateNTP();
  Heartbeat(HEARTBEAT_DURATION, HEARTBEAT_TOPIC, DEVICE_ID);
}
