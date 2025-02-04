// Example testing sketch for various DHT humidity/temperature sensors written by ladyada
// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

// includes padrao
#include <Wire.h>
#include <ArduinoJson.h>
#include "BoardConf.h"
#include "Comm.h"
// includes dos sensores
#include "DHT.h"

// defines padrao
#define INTERV_CAPTURE 5000
#define INTERV_MQTT 1000
#define MSG_SIZE 1024
#define MQTT_TOPIC "IFSCTub/ETCC001"

// pino do sendor de temperatura
#define PIN_DHT 15     // Digital pin connected to the DHT sensor


// objetos padrao
DynamicJsonDocument mqttMsg(MSG_SIZE);
BoardConf board;
Comm comm;

// variavel padrao
char msg[MSG_SIZE];

//------ DHT -------- 
// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(PIN_DHT, DHTTYPE);
//-----------------------------------------------------



void setup() {
  Serial.begin(115200);
  delay(3000);
  
//------ DHT -------- 
  dht.begin();
//-------------------

//------ padrao -------- 
  board.readConf();
  printConf();
  comm.init(BOARD_ESP32_WROOM_32D, board);
  comm.wifiConnect();
  comm.mqttConnect();
//----------------------
  
}

void loop() {
  
  /**********************************************/
  //------------ DHT ----------------------------
  /**********************************************/
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));

  //------- padrao para cada sensor (monta json e envia para mqtt -----------
  /* message pattern for MQTT communication */
  initialize_json();
  mqttMsg["humidity"] = h;
  mqttMsg["temperature"] = t;
  mqttMsg["heat_index"] = hic;
  serialize_and_send("temp_hum_hic");
  delay(INTERV_MQTT);
  //------------------------------------------------


    /**********************************************/
  //------------ ??? ----------------------------
  /**********************************************/

  // inserir os comandos de captura do sensor aqui
  //
  
  //------- padrao para cada sensor (monta json e envia para mqtt -----------
  /* message pattern for MQTT communication */
  //initialize_json();
  //mqttMsg["humidity"] = h;
  //mqttMsg["temperature"] = t;
  //mqttMsg["heat_index"] = hic;
  //serialize_and_send("temp_hum_hic");
  //delay(INTERV_MQTT);
  //------------------------------------------------

  delay(INTERV_CAPTURE);

}

void initialize_json() {
  /* message pattern for MQTT communication */
  mqttMsg.clear();
  mqttMsg["stationId"] = board.station.id;
  mqttMsg["latitude"] = board.station.lat;
  mqttMsg["longitude"] = board.station.lon;
  mqttMsg["type"] = board.station.type;
}

void serialize_and_send(char *measure) {
  char topic[200];

  serializeJson(mqttMsg, msg);
  Serial.println("msg :");
  Serial.println(msg);

  sprintf(topic, "%s/%s", MQTT_TOPIC, measure);
  comm.mqttSetTopic(topic);

  if (comm.mqttPublish(msg, strlen(msg)))
    Serial.println("MQTT: publish success!");
  else
    Serial.println("MQTT: publish error!");
}

void printConf() {
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
}
