// **********************************************************************************
// ESP8266 Pilotage VMC et TEMPO->Traitement des relais VMC TEMPO
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

#ifndef InterRelais_h
#define InterRelais_h
#include <Arduino.h>
#include "Wifinfo.h"
#include "constantes.h"
//#define GetPresentAbsent()  digitalRead(PIN_PRESENT_ABSENT)   //exprès pas de ;

#define HYSTERESIS_TEMPERATURE 20    //en dixièmes

class relais 
{
public:
	relais(void);
	VITESSE_RELAIS getEtatRelaisVitesse(void) const;
	String getEtatRelaisVitesseString(void) const;
	ARRET_MARCHE getEtatReelRelaisMarcheArret(void) const;
	String getEtatReelRelaisMarcheArretString(void) const;
	void traitementRelais(VITESSE_RELAIS Vitesse, ARRET_MARCHE ArretMarche1);
	void tempoChangementEtatRelais(void);
	void  setAujourdhui(ETAT_JOUR  Aujourdhui);
	void  setDemain(VAL_TYPE_JOUR  Demain);
	void   setEtatDemain(ETAT_JOUR  Demain);
	void  setJour_Nuit(ETAT_JOUR  Jour_Nuit);
	ETAT_JOUR getAujourdhui(void) const;
	VAL_TYPE_JOUR getDemain(void) const;
	ETAT_JOUR   getEtatDemain(void) const;
	ETAT_JOUR getJour_Nuit(void) const;
	int32_t getNbMinuteActiveJourCourant(void);
	int32_t getDecompteDelaiCgtVitesse(void)const;
	void setDecompteDelaiCgtVitesse(void);
	void razNbSecondeActiveJourCourant(void);
private:

	ETAT_JOUR  aujourdhui;
	VAL_TYPE_JOUR  demain;
	ETAT_JOUR etatDemain;
	ETAT_JOUR  Jour_Nuit;
	boolean demarrage = false;
	VITESSE_RELAIS etatRelaisVitesse = VITESSE_RELAIS::LENT_REL; //Etat du relais vitesse
	ARRET_MARCHE etatRelaisMarcheArret = ARRET_MARCHE::ARRET_REL; //Etat du relais marche arret
	ARRET_MARCHE etatReelRelaisMarcheArret = ARRET_MARCHE::ARRET_REL; //Etat du relais marche arret	
	int32_t nbSecondeActiveJourCourant = 0;   //pour ventiler un minimum par jour
	int8_t decompteDelaiCgtVitesse = 0;		//pour laisser un délai entre les changement de vitesse
};
extern relais RELAIS;
#endif



