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
#define TEMPO_BIP 15
#define UNEMSENCYCLEHORLOGE F_CPU/1000
#define NBCYCLESOK  10
#define NBCYCLESOKMOINS1  (NBCYCLESOK-1)
class capteur
{
public:
	capteur();
	capteur(int16_t  correction, uint8_t ajustDecimal);
	void  traiteMesure(int16_t mesure); 
	//void  traiteMaxi(int16_t mesure);
	//int16_t getMaxi();
	int16_t getMesureCycle();
	//void clearMesureCycle();
	void clearMesure();
	byte getMesureCycleMsb() const ;
	byte getMesureCycleLsb() const ;
	//float getMesureCycleOled() const ;
	//int16_t getMoyennePeriode() const ;
	//byte getMoyennePeriodeMsb() const ;
	//byte getMoyennePeriodeLsb() const ;
	//float getMoyennePeriodeOled() const ;
	//void  traiteMoyennePeriode(boolean seuil);
	//void setMoyennePourSeuil(int16_t valeur);
	//int16_t getMoyennePourSeuil() const ;
	//byte getMoyennePourSeuilMsb() const ;
	//byte getMoyennePourSeuilLsb() const ;
	//int16_t getMoyennePourSeuilOled() const ;
	uint8_t getEtResetErreur(void);
	void setErreur(uint8_t valeur);
protected:
	int16_t mesureCycle;
//	int16_t moyennePeriode;
//	int32_t cumulPeriode;
//	uint16_t compteurPeriode;
//	int16_t moyennePourSeuil;
	int16_t correction;
	uint8_t ajustDecimal;
//	void traiteMoyennePourSeuil(void);
	uint8_t erreur = 0;
//	int32_t cumulMoyennePourSeuil = 0;
//	uint16_t compteurMoyennePourSeuil = 0;
//	uint8_t compteurCycleMoyennePourSeuil = NBCYCLESOK - 1;
//	int8_t pourcentageSeuil = 0;
//	uint8_t dessous = 0;
//	uint8_t dessus = 0;
//	boolean init = true;
}; 
extern	capteur DHTCUISINE_T;		  //pour traitement
extern	capteur DHTCUISINE_H;		  //pour traitement
#endif //__CAPTEUR_H__
