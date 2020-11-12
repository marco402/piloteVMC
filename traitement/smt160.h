// **********************************************************************************
// ESP8266 Pilotage VMC et TEMPO->mesure de température extérieur avec sonde smt160
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// Written by Marc Prieur (https://marco40github.wixsite.com/website))
//
// History : V1.00 2018-03-23 - First release
//
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

#ifndef SMT160_h
#define SMT160_h
#include <Arduino.h>
#include "Wifinfo.h"
#include "capteur.h"


	class smt160 : public capteur
	{
	public:
		smt160(void);
		smt160(uint8_t pin,int16_t correction,uint8_t ajustDecimal);
		smt160(int16_t correction,uint8_t ajustDecimal);
		void initCapteur(uint8_t pin,int16_t correction,uint8_t ajustDecimal, uint8_t idClasse);
		void setCorrection(int16_t Correction);
		boolean lectureCapteur(void);
		void simuMesure(int16_t capteur);
	private:
		uint8_t pin;
		uint16_t compteurBuzzer = 0;
	};
	extern	smt160 TEMPEXT;

#endif