#define  ESP32  1

#include "Comm.h"

BoardConf board;

Comm comm;

void setup() {
 	//Init Serial USB
 	Serial.begin(115200);
 	Serial.println("Initialize System");

  board.readConf();
  Serial.print("id = ");
  Serial.println(board.station.id);
  Serial.print("lat = ");
  Serial.println(board.station.lat);
  Serial.print("lon = ");
  Serial.println(board.station.lon);
  Serial.print("type = ");
  Serial.println(board.station.type);
  Serial.print("mqttHost = ");
  strcpy(board.station.mqttHost, "localhost");
  Serial.print("mqttIP/Port = ");
  Serial.println(board.station.mqttIP[0]);
  Serial.println(board.station.mqttIP[1]);
  Serial.println(board.station.mqttIP[2]);
  Serial.println(board.station.mqttIP[3]);
  Serial.println(board.station.mqttPort);
  Serial.print("mqttClientId = ");
  Serial.println(board.station.mqttClientId);
  Serial.print("mqttTopic = ");
  //Mobilidade Inteligente e Desenvolvimento Sustentável (MInDS)
  Serial.println(board.station.mqttTopic);
  Serial.print("wifiSsid = ");
  Serial.println(board.station.wifiSsid);
  Serial.print("wifiPwd = ");
  Serial.println(board.station.wifiPwd);
  
  comm.init(BOARD_ESP8266_V3, board);

  comm.wifiConnect();
  
  comm.mqttConnect();
}

void loop() {

 
  delay(20000);
}
