// Includes padrão
#include <Wire.h>
#include <ArduinoJson.h>
#include "BoardConf.h"
#include "Comm.h"
#include "DHT.h"

// Defines padrão
#define INTERV_CAPTURE 15000
#define INTERV_MQTT 5000
#define MSG_SIZE 1024
#define MQTT_TOPIC "IFSCTub/ETCC001"

// Pinos dos sensores
#define PIN_DHT 2         // Pino do sensor DHT
#define PIN_TDS 15        // Pino do sensor TDS
#define PIN_GUVA 34       // Pino do sensor UV
#define PIN_LIGHT 27      // Pino do sensor de luz

// TDS
#define VREF 3.3              // analog reference voltage(Volt) of the ADC
#define SCOUNT  30            // sum of sample point

int analogBuffer[SCOUNT];     // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0;
int copyIndex = 0;

float averageVoltage = 0;
float tdsValue = 0;
float temperature = 25;       // current temperature for compensation
// TDS end

// Objetos padrão
DynamicJsonDocument mqttMsg(MSG_SIZE);
BoardConf board;
Comm comm;

// Variáveis padrão
char msg[MSG_SIZE];

// Objetos dos sensores
DHT dht(PIN_DHT, DHT11); // Inicialização do sensor DHT

// Variáveis específicas dos sensores
float temperature = 25; // Temperatura para compensação
float tdsValue = 0;     // Valor TDS

/*-----------------------------------------------------------
 *
 *  SETUP
 *
 -----------------------------------------------------------*/
void setup() {
  Serial.begin(115200);
  delay(3000);

  // Inicializa os sensores
  initialize_sensors();

  // Configurações padrão
  board.readConf();
  printConf();
  comm.init(BOARD_ESP32_WROOM_32D, board);
  comm.wifiConnect();
  comm.mqttConnect();
}

/*-----------------------------------------------------------
 *
 *  LOOP
 *
 -----------------------------------------------------------*/
void loop() {
  // Captura e envio dos dados de todos os sensores
  capture_dht_data();
  delay(INTERV_MQTT);

  capture_tds_data();
  delay(INTERV_MQTT);
  
  capture_uv_data();
  delay(INTERV_MQTT);
  
  capture_light_data();

  // Intervalo entre capturas
  delay(INTERV_CAPTURE);
}

/*-----------------------------------------------------------
 *
 * initialize_sensors
 * Função para inicializar os sensores
 * 
 -----------------------------------------------------------*/
void initialize_sensors() {
  dht.begin();
  pinMode(PIN_TDS,INPUT);
  Serial.println(F("Sensores inicializados!"));
}

/*-----------------------------------------------------------
 *
 * capture_dht_data
 * Função para capturar dados do DHT
 * 
 -----------------------------------------------------------*/
void capture_dht_data() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  float heatIndex = dht.computeHeatIndex(temperature, humidity, false);

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("Falha na leitura do DHT!"));
    return;
  }

  Serial.printf("DHT - Umidade: %.2f%%, Temperatura: %.2f°C, Índice de calor: %.2f°C\n",
                humidity, temperature, heatIndex);

  // Envia dados para MQTT
  initialize_json();
  mqttMsg["humidity"] = humidity;
  mqttMsg["temperature"] = temperature;
  mqttMsg["heat_index"] = heatIndex;
  serialize_and_send("temp_hum_hic");
}

/*-----------------------------------------------------------
 *
 * capture_tds_data
 * Função para capturar dados do TDS
 * 
 -----------------------------------------------------------*/
void capture_tds_data() {
  float voltage = analogRead(PIN_TDS) * 3.3 / 4096.0;
  float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
  float compensationVoltage = voltage / compensationCoefficient;
  tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage
              - 255.86 * compensationVoltage * compensationVoltage
              + 857.39 * compensationVoltage) * 0.5;

  Serial.printf("TDS - Valor: %.2f ppm\n", tdsValue);

  // Envia dados para MQTT
  initialize_json();
  mqttMsg["tds_value"] = tdsValue;
  serialize_and_send("tds_data");
}

// median filtering algorithm
int getMedianNum(int bArray[], int iFilterLen){
  int bTab[iFilterLen];
  for (byte i = 0; i<iFilterLen; i++)
  bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++) {
    for (i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0){
    bTemp = bTab[(iFilterLen - 1) / 2];
  }
  else {
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  }
  return bTemp;
}

/*-----------------------------------------------------------
 *
 * capture_uv_data
 * Função para capturar dados do sensor UV
 * 
 -----------------------------------------------------------*/
void capture_uv_data() {
  int sensorValue = analogRead(PIN_GUVA);
  float sensorVoltage = sensorValue * 3.3 / 4096.0;

  Serial.printf("UV - Leitura: %d, Tensão: %.2f V\n", sensorValue, sensorVoltage);

  // Envia dados para MQTT
  initialize_json();
  mqttMsg["uv_reading"] = sensorValue;
  mqttMsg["uv_voltage"] = sensorVoltage;
  serialize_and_send("uv_data");
}

/*-----------------------------------------------------------
 *
 * capture_light_data
 * Função para capturar dados do sensor de luz
 * 
 -----------------------------------------------------------*/
void capture_light_data() {
  int analogValue = analogRead(PIN_LIGHT);

  String lightLevel;
  if (analogValue < 40) {
    lightLevel = "Escuro";
  } else if (analogValue < 800) {
    lightLevel = "Fraco";
  } else if (analogValue < 2000) {
    lightLevel = "Claro";
  } else if (analogValue < 3200) {
    lightLevel = "Brilhante";
  } else {
    lightLevel = "Muito Brilhante";
  }

  Serial.printf("Luz - Valor: %d, Nível: %s\n", analogValue, lightLevel.c_str());

  // Envia dados para MQTT
  initialize_json();
  mqttMsg["light_value"] = analogValue;
  mqttMsg["light_level"] = lightLevel;
  serialize_and_send("light_data");
}

/*-----------------------------------------------------------
 *
 * initialize_json
 * Funções utilitárias padrão
 * 
 -----------------------------------------------------------*/
void initialize_json() {
  mqttMsg.clear();
  mqttMsg["stationId"] = board.station.id;
  mqttMsg["latitude"] = board.station.lat;
  mqttMsg["longitude"] = board.station.lon;
  mqttMsg["type"] = board.station.type;
}

/*-----------------------------------------------------------
 *
 * serialize_and_send
 * 
 -----------------------------------------------------------*/
void serialize_and_send(const char *measure) {
  char topic[200];
  serializeJson(mqttMsg, msg);
  sprintf(topic, "%s/%s", MQTT_TOPIC, measure);
  comm.mqttSetTopic(topic);

  if (comm.mqttPublish(msg, strlen(msg))) {
    Serial.println("MQTT: Envio bem-sucedido!");
  } else {
    Serial.println("MQTT: Erro no envio!");
  }
}

/*-----------------------------------------------------------
 *
 * printConf
 * 
 -----------------------------------------------------------*/
void printConf() {
  Serial.printf("Configurações carregadas: id = %s, latitude = %f, longitude = %f\n",
                board.station.id, board.station.lat, board.station.lon);
}
