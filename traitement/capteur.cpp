// **********************************************************************************
// Programme traitement ESP8266 Pilotage VMC et TEMPO->partie commune de gestion des capteurs
// **********************************************************************************
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
// **********************************************************************************
#include <Arduino.h>
#ifndef __AVR__
#include "constantes.h"
#include "Wifinfo.h"
#endif
#include "mySyslog.h"
#include "capteur.h"
#define TABLEAUMOYENNE 0
capteur::capteur(void)
{
}
capteur::capteur(int16_t  correction, uint8_t ajustDecimal)
{
	this->correction = correction;
	this->ajustDecimal = ajustDecimal;
}
void capteur::initCapteur(int16_t  correction, uint8_t ajustDecimal,uint8_t idClasse)
{
	this->correction = correction;
	this->ajustDecimal = ajustDecimal;
	this->idClasse = idClasse;
}
void capteur::setCorrection(int16_t correction)
{
this->correction = correction;
}
int16_t capteur::getCorrection(void) const
{
return this->correction;
}
/// Traitement:
///		mesureCycle
///		cumul de la période.
void  capteur::traiteMesure(int16_t mesure)
{
	mesureCycle=mesure+ correction;   //indice pour correction+correction
	cumulPeriode+=mesureCycle;
	compteurPeriode += 1;
	if (init)
		{
		init = false;
		traiteMoyennePeriode(false);//initialisation de moyenne période dés le premier cycle
		setMoyennePourSeuil(getMoyennePeriode()-2);//initialisation de moyenne pour seuil dés le premier traitement vmc
		//baisse le seuil de 2% pour démarrer
	}
	moyennePeriodeCourante = (moyennePeriodeCourante + mesureCycle) / 2;
}
int16_t  capteur::getMoyennePeriodeCourante(void) const
{
	return moyennePeriodeCourante;
}
void  capteur::clrMoyennePeriodeCourante(void)
{
	moyennePeriodeCourante = 0;
}
void  capteur::traiteMaxi(int16_t mesure)
{
	mesureCycle=mesure+ correction;   //indice pour correction+correction
	if(mesureCycle> cumulPeriode)
		cumulPeriode= mesureCycle;
}
int16_t capteur::getMaxi()
{
	int16_t mesure = cumulPeriode;
	cumulPeriode = 0;
	return mesure;
}
/// appelé dans tous les modes
	/// sur arrêt: les moyenneSeuil vont monter 
	/// sur lent ou rapide forcé, les moyennes seuil vont descendre.
	/// en auto, régulation
	/// traitement moyenne de la periode VMC
	/// cumul moyenne pour les seuils
void  capteur::traiteMoyennePeriode(boolean seuilDynamique)
{
#ifdef DEBUGDHT
	if (seuilDynamique)

		if (idClasse == 1)
		{
			DebugF("compteurPeriode"); Debugln(compteurPeriode);
		}
#endif
	if (compteurPeriode > 0)				//sinon on reste sur la derniere mesure
		moyennePeriode = (int16_t)(cumulPeriode / compteurPeriode);
	else
		return;
	cumulPeriode = 0;
	compteurPeriode = 0;
#if TABLEAUMOYENNE
	mettre dans la classe static 	uint8_t cyclesOK=0;    //pour l'initialisation:1cycle vmc puis 2....jusqu'a NBCYCLESOK
	if (cyclesOK < NBCYCLESOK) 
		cyclesOK += 1;
	dessus = 0;
	dessous = 0;
	if (seuil > 0)
	{
		for (int8_t i = 0; i < cyclesOK; i++)
		{
			if (i == cyclesOK - 1)
				tabMoyennePourSeuil[i] = moyennePeriode;
			else
				tabMoyennePourSeuil[i] = tabMoyennePourSeuil[i + 1];

			if (tabMoyennePourSeuil[i] < moyennePourSeuil)
				dessous += 1;
			else
				dessus += 1;
		}

		if (dessous == cyclesOK)         //tous les cycles au dessous du seuil,on le baisse
		//on pourrait tester la durée de marche ,décalage ou pas de mise a jour si pas en mode auto
			pourcentageSeuil -= 1;
		else if (dessus == cyclesOK)         //tous les cycles au dessus du seuil,on le monte
			pourcentageSeuil += 1;
		TraiteTableauMoyennePourSeuil(seuil);
}
#else
	//traitement pour les seuils attente de NBCYCLESOK * période vmc pour valider les données pour les seuils
	cumulMoyennePourSeuil += moyennePeriode;
	compteurMoyennePourSeuil += 1;
	if (moyennePeriode < moyennePourSeuil)
		dessous += 1;
	else
		dessus += 1;
	compteurCycleMoyennePourSeuil += 1;
		char  buffer[132];
		if (seuilDynamique && idClasse == ID_CLASSES::ID_DHTSDB_H)
		{
			sprintf(buffer, "B:%d\t%d\t%d\t%d\t%d\t%d\n", moyennePeriode, moyennePourSeuil, compteurCycleMoyennePourSeuil, pourcentageSeuil, dessous, dessus);
			Debugln(buffer);
		}
	else if (seuilDynamique && idClasse == ID_CLASSES::ID_DHTCUISINE_H)
		{
			sprintf(buffer, "C:%d\t%d\t%d\t%d\t%d\t%d\n", moyennePeriode, moyennePourSeuil, compteurCycleMoyennePourSeuil, pourcentageSeuil, dessous, dessus);
			Debugln(buffer);
		}
	if (compteurCycleMoyennePourSeuil > NBCYCLESOKMOINS1)
	{	
		//traitement dynamique pour les seuils humidité
		if (seuilDynamique)
		{
			if (dessous > NBCYCLESOKMOINS1)         //tous les cycles au dessous du seuil,on le baisse
				pourcentageSeuil = -1;     //pourcentageSeuil -= 1;
			else if (dessus > NBCYCLESOKMOINS1)         //tous les cycles au dessus du seuil,on le monte
				pourcentageSeuil = 1;	//pourcentageSeuil += 1;
			traiteMoyennePourSeuil();
		}
		cumulMoyennePourSeuil = 0;
		compteurMoyennePourSeuil = 0;
		dessus = 0;
		dessous = 0;
		compteurCycleMoyennePourSeuil = 0;
	}
#endif
}
#if TABLEAUMOYENNE
void capteur::TraiteTableauMoyennePourSeuil(byte seuil)
{
	int32_t sommeMoyennePourSeuil = 0;
	for (int8_t i = 0; i < cyclesOK; i++)
		sommeMoyennePourSeuil += tabMoyennePourSeuil[i];
	moyennePourSeuil = (int16_t)(sommeMoyennePourSeuil / cyclesOK);
	moyennePourSeuil = moyennePourSeuil + (moyennePourSeuil*(int16_t)pourcentageSeuil) / 100;
	//moyennePourSeuilCorrigee=(moyennePourSeuil+seuil-128);
}
#else
//moyennePourSeuil sont les valeurs qui vont etres testées dans TraitementVMC
//Cette moyenne est ajustée en permanence
void capteur::traiteMoyennePourSeuil()
{
	if (compteurMoyennePourSeuil > 0)				//sinon on reste sur la derniere mesure
	{
		moyennePourSeuil = (int16_t)(cumulMoyennePourSeuil / compteurMoyennePourSeuil);
		moyennePourSeuil += (moyennePourSeuil*(int16_t)pourcentageSeuil) / 100;
		//moyennePourSeuilCorrigee=(moyennePourSeuil+seuil-128);
	}
}
#endif
/// getMesureCycle():Mesure sur le cycle en cours.
/// Ce sont les valeurs affichées sur l'écran oled et affichées sur le pc.
/// l'initialisation de mesureCycle n'est pas nécessaire,il faut toutefois appeler clearMesure pour éviter les débordements
// si on appel pas traiteMoyennePeriode .
int16_t capteur::getMesureCycle() const
{
	return mesureCycle;
}
void capteur::clearMesureCycle()
{
	mesureCycle=0;
}
void capteur::clearMesure()  //pas mesureCycle
{
	cumulPeriode = 0;
	compteurPeriode = 0;
}
byte capteur::getMesureCycleMsb() const
{
	return mesureCycle >> 8;
}
byte capteur::getMesureCycleLsb() const
{
	return mesureCycle & 0xFF;
}
float capteur::getMesureCycleOled() const
{
	if (ajustDecimal == 0) return 0.0f;
	return ((float)((float)mesureCycle/(float)ajustDecimal));
}
/// getMoyennePeriode():Mesures moyennées sur la période VMC écoulée.
/// Valeurs utilisées pour les tests dans TraitementVMC et valeurs enregistrées.
int16_t capteur::getMoyennePeriode() const
{
	return moyennePeriode;
}
byte capteur::getMoyennePeriodeMsb() const
{
	return moyennePeriode >> 8;
}
byte capteur::getMoyennePeriodeLsb() const
{
	return moyennePeriode & 0xFF;
}
float capteur::getMoyennePeriodeOled() const
{
	if (ajustDecimal == 0) return 0.0f;
	return((float)((float)moyennePeriode/(float)ajustDecimal));
}
/// MoyennePourSeuil:Ajustement des seuils pour tenir compte des fluctuations normales notemment de l'humidité générale.
int16_t capteur::getMoyennePourSeuil() const
{
	return moyennePourSeuil;
}
byte capteur::getMoyennePourSeuilMsb() const
{
	return moyennePourSeuil >> 8;
}
byte capteur::getMoyennePourSeuilLsb() const
{
	return moyennePourSeuil & 0xFF;
}
int16_t capteur::getMoyennePourSeuilOled() const     //pas en float précision en entier
{
	if (ajustDecimal == 0) return 0.0f;
	return (moyennePourSeuil/(int16_t)ajustDecimal);
}
void capteur::setMoyennePourSeuil(int16_t valeur)
{
	moyennePourSeuil=valeur;
}
uint8_t capteur::getEtResetErreur(void)
{
	uint8_t temp = erreur;
	erreur = 0;
	return temp;
}
void capteur::setErreur(uint8_t valeur)
{
	erreur=valeur;
}
int8_t  capteur::getPourcentageSeuil(void) const		//pour debug
{
	return pourcentageSeuil;
}
