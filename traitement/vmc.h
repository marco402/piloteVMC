// **********************************************************************************
// ESP8266 Pilotage VMC et TEMPO->Traitement de la VMC
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

#ifndef DIVERS_VMC_H
#define DIVERS_VMC_H

#include <Arduino.h> 
#include "Wifinfo.h"
#include "interRelais.h"
#include "constantes.h"
	class vmc
	{
	public:
		vmc(void);
		void TRAITEMENTVMC(void);
		void traiteRazCapteursAuto(void);
		void incrementeCptVmc(void);
		MODES getLeMode(void) const;
		String getLeModeString(void) const;
		void setLeMode(MODES);
		uint16_t  getDecompteTempoArretMarcheForce(void)const;
		MODES getMemoModes(void) const;
		//uint8_t getEtat(void) const;
		uint8_t getSeuilAuto(void) const;
		uint8_t getCasAuto(void) const;
		uint16_t  getSeuilHC(void)const;
		uint16_t  getSeuilHB(void)const;
	private:
		void initialisationMode(void);
		void lectureCapteurs(void);
		void traiteMoyennePeriode(void);
		void testTraitementVmc(void);
		void traitementVMC(void);
		void setCptVmc(uint16_t T);
		int traiteArretMarcheForce(void);
		boolean traitementTempsMiniVMC(void);
		void traitePeriodeVmc(void);
		MODES leMode = MODES::ARRET;  //initialisation identique à affichage
		uint8_t etat = 0; 
		MODES memoModes = MODES::PETITE_VITESSE;    //différent de mode pour entrer dans le traitement dés le premier changement
		uint16_t  decompteTempoArretMarcheForce=0;
		uint16_t cptSecondesVmcAuto= REINIT;
		uint8_t seuilAuto = 0;
		uint8_t casAuto = 0;
		CASSTATUS  info = CASSTATUS::ST_START;  //apres le tableau MODES_AFF de st7735.cpp(affichage)
		int16_t SeuilHC = 0;
		int16_t SeuilHB = 0;
		int8_t CouleurEnCours = 0;
		void traiteEte();
		void traiteHiver();
		//boolean cgtMode = false;
	};
	extern vmc VMC;
	
#endif