// Includes padrão
#include <Wire.h>
#include <ArduinoJson.h>
#include "BoardConf.h"
#include "Comm.h"
#include "DHT.h"

// Defines padrão
#define INTERV_CAPTURE 5000
#define INTERV_MQTT 1000
#define MSG_SIZE 1024
#define MQTT_TOPIC "IFSCTub/ETCC001"

// Pinos dos sensores
#define PIN_DHT 4         // Pino do sensor DHT
#define PIN_TDS 15        // Pino do sensor TDS
#define PIN_GUVA 34       // Pino do sensor UV
#define PIN_LIGHT 36      // Pino do sensor de luz

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

void loop() {
  // Captura e envio dos dados de todos os sensores
  capture_dht_data();
  capture_tds_data();
  capture_uv_data();
  capture_light_data();

  // Intervalo entre capturas
  delay(INTERV_CAPTURE);
}

// Função para inicializar os sensores
void initialize_sensors() {
  dht.begin();
  Serial.println(F("Sensores inicializados!"));
}

// Função para capturar dados do DHT
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
  serialize_and_send("dht_data");
}

// Função para capturar dados do TDS
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

// Função para capturar dados do sensor UV
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

// Função para capturar dados do sensor de luz
void capture_light_data() {
  int lightValue = analogRead(PIN_LIGHT);

  String lightLevel;
  if (lightValue < 40) {
    lightLevel = "Escuro";
  } else if (lightValue < 800) {
    lightLevel = "Fraco";
  } else if (lightValue < 2000) {
    lightLevel = "Claro";
  } else if (lightValue < 3200) {
    lightLevel = "Brilhante";
  } else {
    lightLevel = "Muito Brilhante";
  }

  Serial.printf("Luz - Valor: %d, Nível: %s\n", lightValue, lightLevel.c_str());

  // Envia dados para MQTT
  initialize_json();
  mqttMsg["light_value"] = lightValue;
  mqttMsg["light_level"] = lightLevel;
  serialize_and_send("light_data");
}

// Funções utilitárias padrão
void initialize_json() {
  mqttMsg.clear();
  mqttMsg["stationId"] = board.station.id;
  mqttMsg["latitude"] = board.station.lat;
  mqttMsg["longitude"] = board.station.lon;
  mqttMsg["type"] = board.station.type;
}

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

void printConf() {
  Serial.printf("Configurações carregadas: id = %s, latitude = %f, longitude = %f\n",
                board.station.id, board.station.lat, board.station.lon);
}
