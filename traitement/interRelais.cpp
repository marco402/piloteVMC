// **********************************************************************************
// Programme traitement ESP8266 Pilotage VMC et TEMPO->Traitement des relais VMC TEMPO
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
#include <Arduino.h>
#include "Wifinfo.h"
#include "mySyslog.h"
#include "config.h"
#include "smt160.h"
#include "ledsRGBSerial.h"
#include "constantes.h"
#include "simutempo.h"
#include <WiFiUdp.h>  //pour enregistrement
#include "enregistrement.h"
#include "interRelais.h"
ICACHE_FLASH_ATTR relais::relais()
{
	aujourdhui= ETAT_JOUR_INCONNU;
	demain= INCONNU_J;
	etatDemain= ETAT_JOUR_INCONNU;
	Jour_Nuit= ETAT_JOUR_INCONNU;
//#ifndef SIMUTRAMETEMPO
	pinMode(PIN_RELAIS_VITESSE, OUTPUT);
	digitalWrite(PIN_RELAIS_VITESSE, etatRelaisVitesse);
//#endif
	pinMode(PIN_RELAIS_MARCHE_ARRET, OUTPUT);
	digitalWrite(PIN_RELAIS_MARCHE_ARRET, etatRelaisMarcheArret);
}
void relais::traitementRelais(VITESSE_RELAIS vitesse, ARRET_MARCHE arretMarche)
{
	//**************Si changement d'�tat arr�t marche ou vitesse
	if (((etatRelaisMarcheArret != arretMarche) || (etatRelaisVitesse != vitesse)) && (demarrage == false))
	{
		if (etatReelRelaisMarcheArret != ARRET_MARCHE::ARRET_REL)
		{
			//arr�t pour changer de vitesse ou pour arr�ter
			digitalWrite(PIN_RELAIS_MARCHE_ARRET, ARRET_MARCHE::ARRET_REL);
			//DebuglnF(" relais M/A->arret");
			//fonctionne en remplacant etatreelRelaisMarcheArret par etatRelaisMarcheArret mais incoherence � l'analyse des courbes
			etatReelRelaisMarcheArret = ARRET_MARCHE::ARRET_REL;
			//for�age d'un enregistrement sur changement d'�tat
			//ENREGISTREMENT.setCptSecondesEnregistrement(REINIT);
			ENREGISTREMENT.forceEnregistrementSurMarche();
		}
		etatRelaisMarcheArret = ARRET_MARCHE::ARRET_REL;
		//Changement de vitesse si n�cessaire
		this->etatRelaisVitesse = vitesse;
		//changement de vitesse en sortie
//#ifndef SIMUTRAMETEMPO
		digitalWrite(PIN_RELAIS_VITESSE, etatRelaisVitesse);
		//DebugF(" relais vitesse:"); Debugln((int)etatRelaisVitesse);
//#endif
		//positionnement de l'indicateur pour d�marrage apr�s N secondes(5)
		if (arretMarche == ARRET_MARCHE::MARCHE_REL)
		{
//DebuglnF("demarrage = true");
			demarrage = true;
			decompteDelaiCgtVitesse = TEMPO_CGT_ETAT_RELAIS;
		}
	}
}
//***********traitement de l'activation du relais marche arr�t � appeler toutes les secondes********
//appel� par la boucle principale sans condition
void relais::tempoChangementEtatRelais()
{
	//*********si moteur en marche, increment du compteur de seconde pour ventiler un minimum en fin de journ�e******
	if (etatReelRelaisMarcheArret == ARRET_MARCHE::MARCHE_REL)
		nbSecondeActiveJourCourant += 1;
	//*********Si l'�tat du relais marche arret doit changer et si la temporisation est atteinte,on peux red�marrer
	if (demarrage == true)
	{
		//DebugF("decompteDelaiCgtVitesse:"); Debugln((int)decompteDelaiCgtVitesse);
		//*********Decrementation de la temporisation entre 2 activation des relais***********
		if (decompteDelaiCgtVitesse > -1)		// < TEMPO_CGT_ETAT_RELAIS)		//pour �viter un d�bordement
		{
			decompteDelaiCgtVitesse -= 1;		// += 1;
		}
		if (decompteDelaiCgtVitesse < 0)		//>= TEMPO_CGT_ETAT_RELAIS)
		{
			etatRelaisMarcheArret = ARRET_MARCHE::MARCHE_REL;
			if (etatReelRelaisMarcheArret != ARRET_MARCHE::MARCHE_REL)
			{
				digitalWrite(PIN_RELAIS_MARCHE_ARRET, etatRelaisMarcheArret);
				//DebugF(" relais M/A:"); Debugln((int)etatRelaisMarcheArret);
				etatReelRelaisMarcheArret = etatRelaisMarcheArret;
			}
			//decompteDelaiCgtVitesse = TEMPO_CGT_ETAT_RELAIS;
			demarrage = false;
			ENREGISTREMENT.forceEnregistrementSurMarche();
			//ENREGISTREMENT.setCptSecPeriodeEnregistrement();  //force enregistrement sur marche relais
		}
	}
}
VITESSE_RELAIS relais::getEtatRelaisVitesse()  const //  = VITESSE_RELAIS::LENT_REL; //Etat du relais vitesse
{
	return etatRelaisVitesse;
}
String relais::getEtatRelaisVitesseString()  const //  = VITESSE_RELAIS::LENT_REL; //Etat du relais vitesse
{
		return FPSTR(VIT_REL_AFF[etatRelaisVitesse]) ;
}
ARRET_MARCHE relais::getEtatReelRelaisMarcheArret()  const //  = VITESSE_RELAIS::LENT_REL; //Etat du relais vitesse
{
	return etatReelRelaisMarcheArret;
}
String relais::getEtatReelRelaisMarcheArretString()  const //  = VITESSE_RELAIS::LENT_REL; //Etat du relais vitesse
{
	return FPSTR(ETAT_REL_AFF[etatReelRelaisMarcheArret]);
}
void   relais::setAujourdhui(ETAT_JOUR  aujourdhui)
{
	this->aujourdhui = aujourdhui;
}
void   relais::setDemain(VAL_TYPE_JOUR  demain)
{
	this->demain = demain;
}
void   relais::setEtatDemain(ETAT_JOUR  demain)
{
	this->etatDemain = demain;
}
void   relais::setJour_Nuit(ETAT_JOUR  jour_Nuit)
{
	this->Jour_Nuit = jour_Nuit;
}
ETAT_JOUR relais::getAujourdhui(void) const
{
	return aujourdhui;
}
VAL_TYPE_JOUR relais::getDemain(void) const
{
	return demain;
}
ETAT_JOUR relais::getEtatDemain(void) const
{
	return etatDemain;
}
ETAT_JOUR relais::getJour_Nuit(void) const
{
	return Jour_Nuit;
}

void relais::razNbSecondeActiveJourCourant(void)
{
	nbSecondeActiveJourCourant = 0;
}

int32_t  relais::getNbMinuteActiveJourCourant(void)
{
	return nbSecondeActiveJourCourant / 60;
}
int32_t relais::getDecompteDelaiCgtVitesse(void)const
{
	return decompteDelaiCgtVitesse;
}
void relais::setDecompteDelaiCgtVitesse(void)
{
	decompteDelaiCgtVitesse = TEMPO_CGT_ETAT_RELAIS;
}
