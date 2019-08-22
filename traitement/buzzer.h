// **********************************************************************************
// ESP8266 Pilotage VMC et TEMPO->Gestion du buzzer
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// Written by Marc Prieur (http://mesrealisations.000webhostapp.com/)
//
// History : V1.00 2018-03-23 - First release
//
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

#ifndef BUZZER_H_
#define BUZZER_H_
#include <Arduino.h>
class buzzer
{
	public:
		buzzer(void);
		void setBuzzer(boolean etat);
		uint8_t getBuzzer(void) const;
		void clrBuzzer(void);
	private:
		boolean leBuzzer;
};
extern buzzer BUZZER;

#endif