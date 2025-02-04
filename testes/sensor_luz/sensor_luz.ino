#define LIGHT_SENSOR_PIN 36 // ESP32 pin GIOP36 (ADC0)

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

void loop() {
  // reads the input on analog pin (value between 0 and 4095)
  int analogValue = analogRead(LIGHT_SENSOR_PIN);

  Serial.print("Analog Value = ");
  Serial.print(analogValue);   // the raw analog reading

  // We'll have a few threshholds, qualitatively determined
  if (analogValue < 40) {
    Serial.println(" => Escuro");
  } else if (analogValue < 800) {
    Serial.println(" => Fraco");
  } else if (analogValue < 2000) {
    Serial.println(" => Claro");
  } else if (analogValue < 3200) {
    Serial.println(" => Brilhante");
  } else {
    Serial.println(" => Muito brilhante");
  }

  delay(500);
}
