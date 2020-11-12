// **********************************************************************************
// ESP8266 Pilotage VMC et TEMPO->partie commune de gestion des capteurs
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

#ifndef __CAPTEUR_H__
#define __CAPTEUR_H__

#include <Arduino.h>

#define TEMPO_BIP 15
#define UNEMSENCYCLEHORLOGE F_CPU/1000
#define NBCYCLESOK  10
#define NBCYCLESOKMOINS1  (NBCYCLESOK-1)

class capteur
{
public:

	capteur(void);
	capteur(int16_t  Correction, uint8_t ajustDecimal);
	void initCapteur(int16_t  Correction, uint8_t ajustDecimal, uint8_t idClasse);
	void setCorrection(int16_t Correction);
	int16_t getCorrection(void) const;
	void  traiteMesure(int16_t mesure); 
	void  traiteMaxi(int16_t mesure);
	int16_t getMaxi(void);
	int16_t getMesureCycle(void) const;
	void clearMesureCycle(void);
	void clearMesure(void);
	byte getMesureCycleMsb(void) const;
	byte getMesureCycleLsb(void) const;
	float getMesureCycleOled(void) const;
	int16_t getMoyennePeriode(void) const;
	int16_t getMoyennePeriodeCourante(void) const;
	void clrMoyennePeriodeCourante(void);
	byte getMoyennePeriodeMsb(void) const;
	byte getMoyennePeriodeLsb(void) const;
	float getMoyennePeriodeOled(void) const;
	void  traiteMoyennePeriode(boolean seuil);
	void setMoyennePourSeuil(int16_t valeur);
	int16_t getMoyennePourSeuil(void) const;
	byte getMoyennePourSeuilMsb(void) const;
	byte getMoyennePourSeuilLsb(void) const;
	int16_t getMoyennePourSeuilOled(void) const;
	uint8_t getEtResetErreur(void);
	void setErreur(uint8_t valeur);
	int8_t getPourcentageSeuil(void)const;		//pour debug
protected:
	int16_t mesureCycle;
	int16_t moyennePeriode;
	int16_t moyennePeriodeCourante;
	int32_t cumulPeriode;
	uint16_t compteurPeriode;
	int16_t moyennePourSeuil;
	int16_t correction;
	uint8_t ajustDecimal;
	void traiteMoyennePourSeuil(void);
	uint8_t erreur = 0;
	int8_t pourcentageSeuil = 0;
	int32_t cumulMoyennePourSeuil = 0;
	uint16_t compteurMoyennePourSeuil = 0;
	uint8_t idClasse = 0;
	uint8_t compteurCycleMoyennePourSeuil = NBCYCLESOKMOINS1;
	uint8_t dessous = 0;
	uint8_t dessus = 0;
	boolean init = true;
}; 

extern	capteur CAPTEURIINST;		  //pour traitement
extern	capteur DHTCUISINE_T;		  //pour traitement
extern	capteur DHTCUISINE_H;		  //pour traitement
#endif //__CAPTEUR_H__


