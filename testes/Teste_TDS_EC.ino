/***************************************************
 Este código de exemplo mostra como ler o valor tds e calibrá-lo com a solução padrão.
  707ppm(1413us/cm)a 25^c solução tampão padrão é recomendada.

 Created 2018-1-3
 By Jason <jason.ling@dfrobot.com@dfrobot.com>

 GNU Lesser General Public License.
 See <http://www.gnu.org/licenses/> for details.
 All above must be included in any redistribution.
 ****************************************************/
 
 /***************************************************
   Adaptado e traduzido por Usinainfo
  www.usinainfo.com.br
   ***************************************/
  
 /*********** Nota para Calibrar ***************
 1. Este código é testado no Arduino Uno com Arduino IDE 1.0.5 r2 and 1.8.2.
 2. Calibração na serial:
     enter -> digite "enter" e pressione enter na serial para entrar no modo de calibracao
     cal:"tds_value" -> calibrar com valor tds conhecido em (25^c). ex.= cal:707
     exit -> digite exit para sair e salvar na eepron as info de calibração
 ****************************************************/

#include <EEPROM.h>
#include "GravityTDS.h"

#define TdsSensorPin A0
GravityTDS gravityTds;

float temperature = 25,tdsValue = 0;
float EC;

void setup()
{
    Serial.begin(115200);
    gravityTds.setPin(TdsSensorPin);
    gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
    gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
    gravityTds.begin();  //initialization
}

void loop()
{
    //temperature = readTemperature();  //add your temperature sensor and read it
    gravityTds.setTemperature(temperature);  // set the temperature and execute temperature compensation
    gravityTds.update();  //sample and calculate
    tdsValue = gravityTds.getTdsValue();  // then get the value
    Serial.print("TDS:");
    Serial.print(tdsValue,0);
    Serial.println("ppm");
    Serial.print("EC:");
    EC=((((tdsValue*2))/1000));
    Serial.print(EC);
    Serial.println("mS/cm");
    Serial.println("");
    
    delay(1000);
}
