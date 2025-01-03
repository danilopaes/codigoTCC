/*
  BoardConf.h - Library for read and write EEPROM with info for station
  Created by Felipe Schneider, August 4, 2023.
  Released into the private domain.
*/
#ifndef board_conf_h
#define board_conf_h

#include "Arduino.h"
#include <EEPROM.h>

static const uint8_t SIZE_ID   = 10;
static const uint8_t SIZE_TYPE = 3;
static const uint8_t SIZE_HOST = 50;
static const uint8_t SIZE_MQTT = 100;
static const uint8_t SIZE_WIFI = 50;

class BoardConf
{
  private:
    typedef struct { 
      char  id[SIZE_ID]; 
      float lat;
      float lon;
      char  type[SIZE_TYPE];
      char  mqttHost[SIZE_HOST];
      int   mqttIP[4];
      int   mqttPort;
      char  mqttClientId[SIZE_MQTT];
      char  mqttTopic[SIZE_MQTT];
      int   wifiIP[4];
      char  wifiSsid[SIZE_WIFI];
      char  wifiPwd[SIZE_WIFI];
    } stationInfo;

  public:
    stationInfo station;

    BoardConf();
    void readConf();
    void writeConf();
}; // don't forget the semicolon at the end of the file

#endif
