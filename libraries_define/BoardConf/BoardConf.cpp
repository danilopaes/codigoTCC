/*
  BoardConf.cpp - Library for read and write EEPROM with info for station.
  Created by Felipe Schneider, August 4, 2023.
  Released into the private domain.
*/
#include "Arduino.h"
#include "BoardConf.h"

static const uint8_t ADDR_ID = 0;

BoardConf::BoardConf() {}

void BoardConf::readConf() {
  
  //Init EEPROM
 	EEPROM.begin(sizeof(station));
 	EEPROM.get(ADDR_ID, station);
 	EEPROM.end();
  EEPROM.commit();

}

void BoardConf::writeConf() {
  //Init EEPROM
 	EEPROM.begin(sizeof(station));
	EEPROM.put(ADDR_ID, station);
 	EEPROM.end();
  EEPROM.commit();
}
