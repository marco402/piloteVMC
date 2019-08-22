// **********************************************************************************
// Programme traitement ESP8266 Pilotage VMC et TEMPO->Traitement de la VMC
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
#include <pgmspace.h>
#include <Arduino.h>
#include "Wifinfo.h"
#include "mySyslog.h"
#include "constantes.h"
#include "dht.h"
#include "smt160.h"
#include "ta12.h"
#include "buzzer.h"
#include "config.h"
#include "interRelais.h"
#include "mySntp.h"
#include <WiFiUdp.h>  //pour enregistrement
#include "enregistrement.h"
#include "canBus.h"
#include "vmc.h"
vmc::vmc()
{
}
void vmc::TRAITEMENTVMC(void)
{
	//***************************************Gestion de la mise en marche du relais de d�marrage*************************************
	RELAIS.tempoChangementEtatRelais();
	//**************************************Traitement du mode VMC****************************************************
	//*****************Si le mode a changer, initialisation du nouveau mode
	//ATTENTION,le mode change ici mais le relais marche arret sera command� 5 secondes plus tard-->en attendant:etatRelaisMarcheArret=ArretMarche::ARRET_REL;
	initialisationMode();
	lectureCapteurs();
	testTraitementVmc();
}

void vmc::testTraitementVmc(void)
{
	if (traiteArretMarcheForce() == 0)
	{
			traitePeriodeVmc();
	}
}
//appel� cycliquement sans condition par main_pilote_vmc.cpp
void vmc::setCptVmc(uint16_t T)
{
	cptSecondesVmcAuto = T;
}
void vmc::traiteRazCapteursAuto(void)
{
	if (cptSecondesVmcAuto >= (uint16_t)CONFIGURATION.config.tempo.periode_vmc_sec)  //d�plac� ici sinon capteur de courant a 0
	{
		traiteMoyennePeriode();
		if (leMode != MODES::AUTO)   //pour continuer a traiter les moyennes a la cadence vmc
			cptSecondesVmcAuto = 0;
	}
}
void vmc::traitePeriodeVmc()
{
	if (cptSecondesVmcAuto >= (uint16_t)CONFIGURATION.config.tempo.periode_vmc_sec)
	{
		if (leMode == MODES::AUTO)
		{
			traitementVMC();
			cptSecondesVmcAuto = 0;
		}
		else
		{
			casAuto = 0;	//effacement du status auto
			info = CASSTATUS::ST_START;
			TA12.traitementSeuilTa12();	//true si ok
		}
	}
}
void vmc::incrementeCptVmc()
{
	cptSecondesVmcAuto++;
}
//appel que si on a chang� de mode Marche/arret, petite/grande vitesse ou automatique...
void vmc::initialisationMode()
{
	//noInterrupts();	//au cas ou: framework arduino ???
	if (getMemoModes() != getLeMode())
	{
		decompteTempoArretMarcheForce = 0;				//sinon repart en auto en fin de tempo si on a pas fini le for�age
		switch (leMode)
		{
		case MODES::ARRET:
		{
			RELAIS.traitementRelais(VITESSE_RELAIS::LENT_REL, ARRET_MARCHE::ARRET_REL);
			ENREGISTREMENT.setCptSecPeriodeEnregistrement();		//pour les modes arret, les modes marche sont apr�s la tempo sinon erreur sur marche/arr�t
			break;
		}
		case MODES::PETITE_VITESSE:
		{
			RELAIS.traitementRelais(VITESSE_RELAIS::LENT_REL, ARRET_MARCHE::MARCHE_REL);
			break;
		}
		case MODES::GRANDE_VITESSE:
		{
			RELAIS.traitementRelais(VITESSE_RELAIS::RAPIDE_REL, ARRET_MARCHE::MARCHE_REL);
			break;
		}
		case MODES::TEMPO_ARRET:
			RELAIS.traitementRelais(VITESSE_RELAIS::LENT_REL, ARRET_MARCHE::ARRET_REL);
			ENREGISTREMENT.setCptSecPeriodeEnregistrement();		//pour les modes arret
			decompteTempoArretMarcheForce = CONFIGURATION.config.tempo.duree_forcage_sec;
			break;
		case MODES::TEMPO_MARCHE_PV:
			RELAIS.traitementRelais(VITESSE_RELAIS::LENT_REL, ARRET_MARCHE::MARCHE_REL);
			decompteTempoArretMarcheForce = CONFIGURATION.config.tempo.duree_forcage_sec;
			break;
		case MODES::TEMPO_MARCHE_GV:
			RELAIS.traitementRelais(VITESSE_RELAIS::RAPIDE_REL, ARRET_MARCHE::MARCHE_REL);
			decompteTempoArretMarcheForce = CONFIGURATION.config.tempo.duree_forcage_sec;
			break;
		case MODES::AUTO:
		{
			RELAIS.traitementRelais(VITESSE_RELAIS::LENT_REL, ARRET_MARCHE::ARRET_REL);
			ENREGISTREMENT.setCptSecPeriodeEnregistrement();		//pour les modes arret au d�part
			break;
		}
		case BIDON:
		{
			break;
		}
		}
		setCptVmc(REINIT);	//annule l'attente pour le traitement vmc reste a cette valeur si pas en auto-->traitement a chaque cycle
		memoModes = leMode;
		TA12.clrMoyennePeriodeCourante();
		//interrupts();
	}
}
uint16_t  vmc::getDecompteTempoArretMarcheForce(void)const
{
	return decompteTempoArretMarcheForce;
}
int vmc::traiteArretMarcheForce()
{
	if (decompteTempoArretMarcheForce == 0)
	{
		return 0;
	}
	else if (decompteTempoArretMarcheForce == 1)
	{
		leMode = MODES::AUTO;
		decompteTempoArretMarcheForce = 0;			//on perd 1 sec sur la tempo...
		return 1;
	}
	else
	{
		decompteTempoArretMarcheForce -= 1;
		return 1;
	}
}
boolean vmc::traitementTempsMiniVMC(void)
{
	// ok Nbminute=HEURE.tm.Hour*60+HEURE.tm.Minute;
	// ok NbMinuteAvantMinuit=1440-LECTURESERIAL.tempsVmcMinParJour+getNbMinuteActiveJourCourant(); 
	//if ((HEURE.tm.Hour==23) && (HEURE.tm.Minute>=(60-(LECTURESERIAL.tempsVmcMinParJour-(NbSecondeActiveJourCourant/60)))))        //il est temps de ventiler jusqu'a minuit
	// OK if (((int)(HEURE.tm.Hour*60)+HEURE.tm.Minute)>=1425)    //(1440-((int)LECTURESERIAL.tempsVmcMinParJour-(NbSecondeActiveJourCourant/60))) && (leMode==AUTO)	)        //il est temps de ventiler jusqu'a minuit
	if (((Clock.getHour() * 60 + Clock.getMinute() ) > (1339 - CONFIGURATION.config.tempo.duree_mini_sec/60.0 + RELAIS.getNbMinuteActiveJourCourant())) && (leMode == AUTO))
	{
		RELAIS.traitementRelais(VITESSE_RELAIS::LENT_REL, ARRET_MARCHE::MARCHE_REL);   //toujours lent en mode auto ou forc� pour temps mini
		return true;
	}
	return false;
}
MODES vmc::getLeMode() const
{
	return leMode;
}
String vmc::getLeModeString() const
{
	//return (char *)pgm_read_dword(&(MODES_AFF[leMode]));
	return FPSTR(MODES_AFF[leMode]);
}
void vmc::setLeMode(MODES lemode)
{
	leMode = lemode;
}
MODES vmc::getMemoModes() const
{
	return memoModes;
}
void vmc::traitementVMC(void)
{
	//param�trage via page web
		//seuil chaud(23)
		//seuil froid(25)
		//pourcent_hum(15)
		//p�riode vmc(300)

	//initialisation
		//MoyennePourSeuil=70

	//fonctions appel�es:
		//a la p�riode vmc --->traiteMoyennePeriode(1)
			//dans traiteMoyennePeriode(1)---> traiteMoyennePourSeuil() si compteurCycleMoyennePourSeuil == NBCYCLESOK(10)

	//mode automatique
	//	uniquement  petite vitesse
		//cas �t�, limiter l'entr�e d'air chaud et l'expulsion d'air plus frais-->�volution des seuils humidit�	SeuilHC et SeuilHB
	//cas trait�s dans l'ordre:
		//1-cas hiver, limiter l'entr�e d'air froid  et l'expulsion d'air chaud			temp�rature ext > seuil chaud------------>arret
				//en modifiant les seuils humidit� proportionellement � temp�rature cuisine - temp�rature ext�rieur
		//2-cas humidit� cuisine trop �lev� par rapport � un seuil �volutif	SeuilHC---------------------------------------------->marche
		//3-cas humidit� salle de bain trop �lev� par rapport � un seuil �volutif SeuilHB---------------------------------------->marche
		//4-sinon---------------------------------------------------------------------------------------------------------------->arret

//	Pour l'humidit�, le principe de seuil fixe ne fonctionne pas, il faut adapter le seuil en fonction de "l'humidit�e g�n�rale"

	//traitement des seuil SeuilHC et SeuilHB
		//si la temp�rature int�rieure <= seuil froid
			//Seuil=humidit�e moyenne pour seuil+(temp�rature moyenne-temp�rature ext�rieur)/pourcent_hum

	//traitement de humidit�e moyenne pour seuil
		//si la temp�rature moyenne est > temp�rature pour seuil 10 cycles vmc cons�cutif, le pourcentageSeuil(diff�rent de pourcent_hum)=+1
		//si la temp�rature moyenne est < temp�rature pour seuil 10 cycles vmc cons�cutif, le pourcentageSeuil =-1
		
		//dans ces 2 cas, on recalcul la moyenne pour seuil = moyennePourSeuil + (moyennePourSeuil * pourcentageSeuil) / 100;

		//le pourcentageSeuil seuil �volue donc au minimum tous les 300 * 10 secondes


		//DebugF("DHTCUISINE_T.getMoyennePeriode: "); Debugln(DHTCUISINE_T.getMoyennePeriode());

	SeuilHC = DHTCUISINE_H.getMoyennePourSeuil();
	SeuilHB = DHTSDB.DHT_H.getMoyennePourSeuil();
		////DebugF("SeuilHCav: "); Debugln(SeuilHC);
		////DebugF("SeuilHBav: "); Debugln(SeuilHB);

		////if ((DHTCUISINE_T.getMoyennePeriode()<= CONFIGURATION.config.tempo.seuil_temp_froid_dixieme_degres) && (CONFIGURATION.config.tempo.pourcent_hum > 0))  //c'est l'hiver:moins d'air frais si...
		////	SeuilHC += (DHTCUISINE_T.getMoyennePeriode() - TEMPEXT.getMoyennePeriode()) / (CONFIGURATION.config.tempo.pourcent_hum*10);//*10 temperature en dixi�me
		////if ((DHTSDB.DHT_T.getMoyennePeriode() <= CONFIGURATION.config.tempo.seuil_temp_froid_dixieme_degres) && (CONFIGURATION.config.tempo.pourcent_hum > 0))  //c'est l'hiver:moins d'air frais si...
		////	SeuilHB += (DHTSDB.DHT_T.getMoyennePeriode() - TEMPEXT.getMoyennePeriode()) / (CONFIGURATION.config.tempo.pourcent_hum*10);//*10 temperature en dixi�me
		///*	DebugF("SeuilHC: "); Debugln(SeuilHC);
		//	DebugF("SeuilHB: "); Debugln(SeuilHB);*/
		//DebugF("TEMPEXT.getMoyennePeriode(): "); Debugln(TEMPEXT.getMoyennePeriode());
		//DebugF("DHTCUISINE_H.getMoyennePeriode(): "); Debugln(DHTCUISINE_H.getMoyennePeriode());
		//DebugF("DHTSDB.DHT_H.getMoyennePeriode(): "); Debugln(DHTSDB.DHT_H.getMoyennePeriode());
		//DebugF("seuil_temp_chaud_dixieme_degres: "); Debugln(CONFIGURATION.config.tempo.seuil_temp_chaud_dixieme_degres);
		//DebugF("seuil_temp_froid_dixieme_degres: "); Debugln(CONFIGURATION.config.tempo.seuil_temp_froid_dixieme_degres);
		//boolean tropFroidDehors = (TEMPEXT.getMoyennePeriode() < CONFIGURATION.config.tempo.seuil_temp_froid_dixieme_degres);//c'est l'hiver trop froid dehors risque de ne plus ventiler
		boolean tropChaudDehors = (TEMPEXT.getMoyennePeriode() > CONFIGURATION.config.tempo.seuil_temp_chaud_dixieme_degres);
		boolean cuisineTropHumide = (DHTCUISINE_H.getMoyennePeriode() > SeuilHC);
		boolean salledebainTropHumide = (DHTSDB.DHT_H.getMoyennePeriode() > SeuilHB);
		if (tropChaudDehors)
		{
			RELAIS.traitementRelais(VITESSE_RELAIS::LENT_REL, ARRET_MARCHE::ARRET_REL );							//c'est l'�t� trop chaud dehors seuil de temp�rature qui permet de ventiler la nuit.
			casAuto = CASSTATUS::ST_TROPCHAUD;
			seuilAuto = CONFIGURATION.config.tempo.seuil_temp_chaud_dixieme_degres/10;
			//DebuglnF("2");
		}
		else if (cuisineTropHumide)
		{
			RELAIS.traitementRelais(VITESSE_RELAIS::LENT_REL, ARRET_MARCHE::MARCHE_REL);						//trop d'humidit� cuisine
			casAuto = CASSTATUS::ST_CUISTROPHUM;
			seuilAuto = SeuilHC;
			//DebuglnF("3");
		}	
		else if (salledebainTropHumide)
			{
			RELAIS.traitementRelais(VITESSE_RELAIS::LENT_REL, ARRET_MARCHE::MARCHE_REL);						//trop d'humidit� salle de bain
			casAuto = CASSTATUS::ST_SDBTROPHUM;
			seuilAuto = SeuilHB;
			//DebuglnF("4");
			}
		else //if (RELAIS.getEtatReelRelaisMarcheArret()== ARRET_MARCHE::MARCHE_REL)
			{
			RELAIS.traitementRelais(VITESSE_RELAIS::LENT_REL, ARRET_MARCHE::ARRET_REL);   						//arret si aucun cas demande de ventiler
			casAuto = ST_START; //affichage du nombre de minute restante
			seuilAuto= 0;
			//DebuglnF("5");
			}
		//DebugF("casAuto: "); Debugln(casAuto);
		//DebugF("seuilAuto: "); Debugln(seuilAuto);
}
//appel� quand cptSecondesVmcAuto>=LECTURESERIAL.periode_vmc
void vmc::traiteMoyennePeriode(void)
{
		DHTSDB.DHT_T.traiteMoyennePeriode(false);
		DHTSDB.DHT_H.traiteMoyennePeriode(true);
#ifdef DEBUGDHT
		DebugF("vmc:mps"); Debugln((uint8_t)DHTSDB.DHT_H.getMoyennePourSeuil());
		DebugF("vmc_%s:"); Debugln(DHTSDB.DHT_H.getPourcentageSeuil());
#endif
		DHTCUISINE_T.traiteMoyennePeriode(false);  //capteurs distants
		DHTCUISINE_H.traiteMoyennePeriode(true);  //capteurs distants
		TEMPEXT.traiteMoyennePeriode(false);
		TA12.traiteMoyennePeriode(false);
		//DebugF("1 TA12.getMoyennePeriode: "); Debugln(TA12.getMoyennePeriode());
		//DebugF("1 TA12.getPuissanceConsommee: "); Debugln(TA12.getPuissanceConsommee());
}
void vmc::lectureCapteurs(void)
{
	DHTSDB.lectureCapteur();
	TEMPEXT.lectureCapteur();
	if (CAN_BUS.getReceptionCapteurs())
	{
	DHTCUISINE_T.traiteMesure(CAN_BUS.getTCuis());
	DHTCUISINE_H.traiteMesure(CAN_BUS.getHCuis());
	CAN_BUS.resetReceptionCapteurs();
	}
}
uint8_t vmc::getSeuilAuto(void) const
{
	return seuilAuto;
}
uint8_t vmc::getCasAuto(void) const
{
	return casAuto;
}
uint16_t  vmc::getSeuilHC(void)const
{
	return SeuilHC;
}
uint16_t  vmc::getSeuilHB(void)const
{
	return SeuilHB;
}




