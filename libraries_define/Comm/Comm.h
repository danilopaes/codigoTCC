/*
  Comm.h - Library for LoRa, Wi-Fi and MQTT communication.
  Created by Felipe Schneider, August 4, 2023.
  Released into the private domain.
*/
#ifndef comm_h
#define comm_h

#include "Arduino.h"

#ifdef ESP32
 #include <WiFi.h>
#else
 #include <ESP8266WiFi.h>
#endif

#include "PubSubClient.h"
#include "BoardConf.h"

static const int BOARD_ESP8266_V1 = 1;
static const int BOARD_ESP8266_V2 = 2;
static const int BOARD_ESP8266_V3 = 3;
static const int BOARD_ESP32_WROOM_32D = 4;
static const int BOARD_ESP_LORA_V2 = 5;

class Comm
{
  private:
    int           board_type;
    
    char          *wifiSsid;
    char          *wifiPwd;
    WiFiClient    wifiClient;

    char          *mqttHost;
    IPAddress     mqttIP;
    int           mqttPort = 1883;
    char          *mqttClientId;
    char          *mqttTopic;
    PubSubClient  mqttClient;

  public:
    Comm();

    void init(int board_type, BoardConf board);   
    void mqttConnect();
    boolean mqttPublish(char *msg, int len);
    void mqttSubscribe();
    void mqttSetTopic(char *topic);
    
    void wifiConnect();
    boolean wifiConnected();

}; // don't forget the semicolon at the end of the file

#endif
