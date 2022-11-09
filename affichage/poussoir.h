// **************************************************************************************
// ESP8266 Pilotage VMC et TEMPO->Gestion d'un poussoir sensitif pour changer de mode VMC
// **************************************************************************************
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
// **************************************************************************************
#ifndef POUSSOIR_H_
#define POUSSOIR_H_
#include <Arduino.h>
class poussoir
{
	public:
		poussoir();
		boolean traitement(MODES forcageMode, int16_t dureeTempo);
		MODES getLeMode(void);
		void clearLeMode(void);
		void setLemode(unsigned char leMode);
		unsigned long  getTempsMilliCommandeTemporisees(void);
//#ifdef TRAITMODE
//		void testModeForce(int16_t decompteTempoArretMarcheForce);   // , uint16_t duree_forcage_sec);
//#endif
	private:
		MODES modeTransitoire= MODES::AUTO;   //ARRET
		MODES memoModes= MODES::AUTO;
		MODES leMode= MODES::AUTO;
		unsigned long tempsMilli = 0;
		unsigned long tempsMilliCommandeTemporisees = 0;
};
extern poussoir POUSSOIR;
#endif /* POUSSOIR_H_ */
