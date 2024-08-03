#ifndef ALIYUN_H
#define ALIYUN_H

#include <ArduinoJson.h>
#include "PubSubClient.h"
#include "WiFi.h"

#define WIFI_SSID              "507"
#define WIFI_PASSWORD          "507507507507"

#define MQTT_SERVER       "iot-06z00fiose4kd7h.mqtt.iothub.aliyuncs.com"
#define MQTT_PORT          1883 //¶Ë¿ÚºÅ
#define MQTT_USERNAME     "MQTT_ESP32&k1lvkKwHIIA"
#define CLIENT_ID         "k1lvkKwHIIA.MQTT_ESP32|securemode=2,signmethod=hmacsha256,timestamp=1722584918367|"
#define MQTT_PASSWORD     "19dba5e78f34e8280a94240daae4cb3c12ef3e9f0e6f53e6e90715dd543f3db8"

#define PubTopic          "/sys/k1lvkKwHIIA/MQTT_ESP32/thing/event/property/post"
//#define PubTopic_computer "/k1lvkKwHIIA/MQTT_ESP32/user/send"
#define SubTopic          "/k1lvkKwHIIA/MQTT_ESP32/user/get"

extern WiFiClient espClient;
extern PubSubClient client;

void processJsonMessage(char* message);
void callback(char* topic, byte* payload, unsigned int length);
void WifiConnect();
void MQTTCheckConnect();
void pubMsg();
void SendLED_Msg(int *last_Brightness,int *Brightness);

#endif