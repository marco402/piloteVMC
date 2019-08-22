// **************************************************************************************
// ESP8266 Pilotage VMC et TEMPO->Gestion d'un poussoir sensitif pour changer de mode VMC
// **************************************************************************************
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
// **************************************************************************************
#ifndef POUSSOIR_H_
#define POUSSOIR_H_
#include <Arduino.h>
class poussoir
{
	public:
		poussoir();
		boolean traitement();
		MODES getLeMode(void);
		void clearLeMode(void);
		void setLemode(unsigned char leMode);
	private:
		MODES modeTransitoire= MODES::ARRET;
		MODES memoModes= MODES::ARRET;
		MODES leMode= MODES::ARRET;
};
extern poussoir POUSSOIR;
#endif /* POUSSOIR_H_ */