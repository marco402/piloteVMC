// ***************************************************************************************************************
// ESP8266 Pilotage VMC et TEMPO->Mesure et surveillance du courant consommé par le moteur VMC avec un TA12-200.
// ***************************************************************************************************************
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



#ifndef TA12_H_
#define TA12_H_

#include <Arduino.h>
#include "Wifinfo.h"

#define TEST_CAPTEURS 0	
	class ta12 : public capteur
	{
	public:
		ta12(void);
		ta12(uint8_t pin,uint8_t ajustDecimal);
		void initCapteur(uint8_t pin, uint8_t ajustDecimal);
		boolean traitementLectureTa12(void);
		boolean traitementSeuilTa12(void);
		void setSeuilPetiteVitesse(int16_t seuil);
		void setSeuilGrandeVitesse(int16_t seuil);
		int16_t getSeuilPetiteVitesse(void) const;
		int16_t getSeuilGrandeVitesse(void) const;
		uint8_t getPuissanceConsommee(void) const;
	private:
		void lectureZeroCapteur(void);
		void lectureCapteur(void);
		uint16_t ta12_lectureTA12(void);
		uint8_t pin;
		int capteur_ta12;
		uint16_t zeroCapteur;
		int seuilPetiteVitesse=-1;
		int seuilGrandeVitesse = -1;
		int seuilPetiteVitesseMin = -1;
		int seuilGrandeVitesseMin = -1;
		int seuilPetiteVitesseMax = -1;
		int seuilGrandeVitesseMax = -1;
	};
	extern ta12 TA12;

#endif /* TA12_H_ */