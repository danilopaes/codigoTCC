 /*
  GUVA-S12SD-UV-Module
  Made on 03 Feb 2020
  by Amir Mohammad Shojaee @ Electropeak
  Home

*/

#define PIN_GUVA 15 

void setup() 
{
  Serial.begin(115200);
}
 
void loop() 
{
  float sensorVoltage; 
  float sensorValue;
 
  sensorValue = analogRead(PIN_GUVA);
  sensorVoltage = sensorValue/1024*5.0;
  Serial.print("sensor reading = ");
  Serial.print(sensorValue);
  Serial.print("        sensor voltage = ");
  Serial.print(sensorVoltage);
  Serial.println(" V");
  delay(1000);
}
