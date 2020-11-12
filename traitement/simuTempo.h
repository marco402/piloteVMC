// **********************************************************************************
// ESP8266 Pilotage VMC et TEMPO->Simulation de messages tempo version historique
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

#ifndef SIMU_TEMPO_H
#define SIMU_TEMPO_H

#include <Arduino.h>
#include "Wifinfo.h"

#define END_STR '\0' // End of group 
#define NBCHAR  23
#define NBCHAMP  17 
#define SerialSimu Serial1
enum lesIndices { STX, CPTJBN, CPTJWN, CPTJRN, CPTJBJ, CPTJWJ, CPTJRJ, ADCO, OPTARIF, ISOUSC, PTEC, DEMAIN, IINST, IMAX, HHPHC, MOTDETAT, HCJB, HPJB, HPJW, HCJR, HPJR, DERNIER
};

class SimuTempo
{
public:
	void initSimuTrameTempo(void);
	void traite1Trame(  unsigned long secondes);
private:
	void emetTrameTempo(  unsigned long secondes, int compteurCourant,unsigned int  couleurDemain);
	int randMinMax(int min, int max);
	void traite1champ(char * trame, int longueur, int longueurEntete, int indice, char * buffer);
	void traite1champ(char * trame, char * format, int longueur, int longueurEntete, int indice, unsigned long variable);
};
extern SimuTempo SIMU_TEMPO;

#endif