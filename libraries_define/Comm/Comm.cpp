/*
  Comm.cpp - Library for Wi-Fi and MQTT communication.
  Created by Felipe Schneider, August 4, 2023.
  Released into the private domain.
*/
#include "Arduino.h"
#include "Comm.h"

Comm::Comm() {
    mqttHost = (char *)malloc(sizeof(char) * (SIZE_HOST + 1));
    mqttClientId = (char *)malloc(sizeof(char) * (SIZE_MQTT + 1));
    mqttTopic = (char *)malloc(sizeof(char) * (SIZE_MQTT + 1));
    wifiSsid  = (char *)malloc(sizeof(char) * (SIZE_WIFI + 1));
    wifiPwd  = (char *)malloc(sizeof(char) * (SIZE_WIFI + 1));
}

void Comm::init(int board_type, BoardConf board) {
  
    this->board_type = board_type;
    strcpy(mqttHost, board.station.mqttHost);
    mqttIP[0] = board.station.mqttIP[0];
    mqttIP[1] = board.station.mqttIP[1];
    mqttIP[2] = board.station.mqttIP[2];
    mqttIP[3] = board.station.mqttIP[3];
    mqttPort  = board.station.mqttPort;
    strcpy(mqttClientId, board.station.mqttClientId);
    strcpy(mqttTopic, board.station.mqttTopic);   
    strcpy(wifiSsid, board.station.wifiSsid);
    strcpy(wifiPwd, board.station.wifiPwd);
    
}


void Comm::wifiConnect() {
  if (WiFi.status() != WL_CONNECTED) {
      Serial.print("Connecting to ");
      Serial.println(wifiSsid);
    
      WiFi.mode(WIFI_STA);
      WiFi.begin(wifiSsid, wifiPwd);
     
      while (WiFi.status() != WL_CONNECTED) {
        delay(3000);
        Serial.print(".");
      }
      
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
  }
}

boolean wifiConnected() {
  if (WiFi.status() == WL_CONNECTED) 
      return true;
  else
      return false;
}

void Comm::mqttConnect() {

  mqttClient.setClient(wifiClient);
  
  if (!mqttClient.connected()) {
      Serial.println("MQTT set server...");
      mqttClient.setServer(mqttIP, mqttPort);
    
      Serial.println("MQTT connection...");
      while (!mqttClient.connected()) {
        int len = strlen(mqttClientId);
        Serial.println("MQTT testing connection...");
        Serial.print("mqttClientId (");
        Serial.print(len);
        Serial.print(") = [");
        Serial.print(mqttClientId);
        Serial.print("]");
        
        if (mqttClient.connect(mqttClientId)) {
          Serial.println("MQTT connected...");
          Serial.print(mqttClientId);
          Serial.println(" : Connected to MQTT Broker...");
        } else {
          Serial.print("failed, rc=");
          Serial.print(mqttClient.state());
          Serial.println(" try again in 0.5 seconds");
          Serial.print(".");
          delay(3000);
        }
      }
  }
}

boolean Comm::mqttPublish(char *msg, int len) {
    boolean result = false;
    
    Serial.println("MQTT publish...");
    Serial.print(mqttTopic);
    Serial.println(len);

    result = mqttClient.publish(mqttTopic, (byte*)msg, len);

    mqttClient.loop();

    return result;
}

void Comm::mqttSetTopic(char *topic) {
  strcpy(mqttTopic, topic);     
}
