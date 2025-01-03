#include "BoardConf.h"

BoardConf board;

void setup() {
  
 	//Init Serial USB
 	Serial.begin(115200);
 	Serial.println("Initialize System");

 	Serial.print("Write data...: ");
  strcpy(board.station.id, "ETCC001");
  board.station.lat = -28.4741;
  board.station.lon = -49.0254;
  strcpy(board.station.type, "SE");
  strcpy(board.station.mqttHost, "192.168.0.101");
  board.station.mqttIP[0] = 192;
  board.station.mqttIP[1] = 168;
  board.station.mqttIP[2] = 0;
  board.station.mqttIP[3] = 101;
  board.station.mqttPort = 1883;
  strcpy(board.station.mqttClientId, board.station.id);
  //Mobilidade Inteligente e Desenvolvimento Sustentável (MInDS)
  sprintf(board.station.mqttTopic, "%s%s", "IFSCTub/", board.station.id);
  board.station.wifiIP[0] = 0;
  board.station.wifiIP[1] = 0;
  board.station.wifiIP[2] = 0;
  board.station.wifiIP[3] = 0;
  strcpy(board.station.wifiSsid, "ProjetoTCC");
  strcpy(board.station.wifiPwd, "danilo123");

  board.writeConf(); 
}

void loop() {

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
  Serial.print(board.station.mqttIP[0]);
  Serial.print(board.station.mqttIP[1]);
  Serial.print(board.station.mqttIP[2]);
  Serial.print(board.station.mqttIP[3]);
  Serial.println(board.station.mqttPort);
  Serial.print("mqttClientId = ");
  Serial.println(board.station.mqttClientId);
  Serial.print("mqttTopic = ");
  //Mobilidade Inteligente e Desenvolvimento Sustentável (MInDS)
  Serial.println(board.station.mqttTopic);
  Serial.print("wifiIP = ");
  Serial.println(board.station.wifiIP[0]);
  Serial.println(board.station.wifiIP[1]);
  Serial.println(board.station.wifiIP[2]);
  Serial.println(board.station.wifiIP[3]);
  Serial.print("wifiSsid = ");
  Serial.println(board.station.wifiSsid);
  Serial.print("wifiPwd = ");
  Serial.println(board.station.wifiPwd);
   
  delay(20000);
}
