
// **********************************************************************************
// Programme traitement ESP8266 Teleinfo Traitement du message pour  enregistrement
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
// This function call LibTeleinfo function
//	Written by Charles-Henri Hallard (http://hallard.me)
//
// All text above must be included in any redistribution.
//
// **********************************************************************************
//****06/2022 modification si changement de portEnr en fonctionnement
//***********************************************************************************
#include <Arduino.h>
#include <WiFiUdp.h>
#include "Wifinfo.h"
#include "mySyslog.h"
#include "constantes.h"
#include "config.h"
#include "LibTeleinfo.h"
#include "interRelais.h"
#include "dht.h"
#include "vmc.h"
#ifdef TA12
	#include "ta12.h"
#endif
#include "smt160.h"
#include "mySNTP.h"
#include "myWifi.h"
#include "webclient.h"
//#include "mySntp.h"
#include "myTinfo.h"
#include "enregistrement.h"
#include <ESP8266WiFi.h>
#define NO_DEBUGENREGISTREMENT

//boolean forceEnregistrement = false;
enregistrement::enregistrement()
{}
void ICACHE_FLASH_ATTR enregistrement::init()
{
#ifndef EMISSION_ENREGISTREMENT
	nbMessageEmis = 1; //limite a 1 message
#endif	
	 //uint8_t un=atoi(CONFIGURATION.config.tempo.host_enr[0]);
  // uint8_t deu=atoi(CONFIGURATION.config.tempo.host_enr[1]);
  // uint8_t troi=atoi(CONFIGURATION.config.tempo.host_enr[2]);
  // uint8_t quat=atoi(CONFIGURATION.config.tempo.host_enr[3]); 
   //adressIP(un,deu,troi,quat);
   memoPort = CONFIGURATION.config.tempo.portEnr;
   udpEnr.begin(memoPort);  //port a ecouter localement;//pointeurChar = 0;
}
void ICACHE_FLASH_ATTR enregistrement::stop()
{
	udpEnr.stop(); 
}
void enregistrement::TRAITEENREGISTREMENT( boolean nouvelleTrame,   boolean cgtCompteur, uint16_t dureeMax)
{

	//enregistre qu'a la premiere mise en marche relais apres changement de compteur???

	this->dureeMax = dureeMax;
	if ((cptSecPeriodeEnregistrement >= CONFIGURATION.config.tempo.periode_enr)  || cgtCompteur  || (premiersEnregistrement < 6)  || forceEnregistrement)   //|| nouvelleTrame
	{
		enregistreMessageTempoVmc();  //
	}
	incCptSecPeriodeEnregistrement();
#ifdef EMISSION_ENREGISTREMENT
	if (WIFIOK)
	{
		testAcquitementMessage();
	}
#endif
	forceEnregistrement = false;
}
void enregistrement::incCptSecPeriodeEnregistrement(void)
{
	cptSecPeriodeEnregistrement++;
}
uint16_t enregistrement::getpremiersEnregistrement(void)
{
	return premiersEnregistrement;
}
void enregistrement::clrCptSecPeriodeEnregistrement(void)
{
	cptSecPeriodeEnregistrement= 0;
}
void enregistrement::setCptSecPeriodeEnregistrement(void)
{
	cptSecPeriodeEnregistrement= CONFIGURATION.config.tempo.periode_enr;
}
void enregistrement::clrPremierEnregistrement(void)
{
	premiersEnregistrement = 0;
}
//typedef enum { E_HEURE = 0, E_MINUTES, E_SECONDES, E_JOUR_DU_MOIS, E_TEMPEXTMSB, E_TEMPEXTLSB, E_TEMPINTMSB, E_TEMPINTLSB, E_CO, E_HUMIDITE, E_MODE_RELAIS, E_TA12, E_TEMPEXTMSB_2, E_TEMPEXTLSB_2, E_TEMPINTMSB_2, E_TEMPINTLSB_2, E_CO_2, E_HUMIDITE_2, E_NBDATA }champsData;
#define NO_TESTVMC
void enregistrement::enregistreMessageTempoVmc()
{
	//*******************************************************************
#ifdef 	WITHWIFINFO
	if (traiteCompteursTempo(&lesMessages[pointeurEcritureMessage], premiersEnregistrement))
	{
#endif
    //DebugF("compteur: "); Debugln((uint32_t)lesMessages[pointeurEcritureMessage].compteur);
		//****************************************************
		lesMessages[pointeurEcritureMessage].heure1970 = Clock.getTimeSeconds();
		lesMessages[pointeurEcritureMessage].dureeMax = this->dureeMax;
		lesMessages[pointeurEcritureMessage].tempExt = TEMPEXT.getMoyennePeriode();
		lesMessages[pointeurEcritureMessage].tempSdb = DHTSDB.DHT_T.getMoyennePeriode();
		lesMessages[pointeurEcritureMessage].humCuis = DHTCUISINE_H.getMoyennePeriodeLsb();//0 e 100--->7 bits
		lesMessages[pointeurEcritureMessage].humSdb = DHTSDB.DHT_H.getMoyennePeriodeLsb();
#ifdef TESTVMC
		lesMessages[pointeurEcritureMessage].iInstMax = DHTSDB.DHT_H.getPourcentageSeuil();
		//DebugF("enr_%s:"); Debugln(DHTSDB.DHT_H.getPourcentageSeuil());
		lesMessages[pointeurEcritureMessage].tempCuis = DHTSDB.DHT_H.getMoyennePourSeuil()*10;
		//DebugF("enr_ms:"); Debugln(DHTSDB.DHT_H.getMoyennePourSeuil());
#else
		lesMessages[pointeurEcritureMessage].iInstMax = CAPTEURIINST.getMaxi();
		lesMessages[pointeurEcritureMessage].tempCuis = DHTCUISINE_T.getMoyennePeriode();
#endif
		//lesMessages[pointeurEcritureMessage].iVmc = TA12.getMesureCycleLsb();	//maxi 1024  10 bits  8 bits doivent etres suffisants---256  environ 600w
		//lesMessages[pointeurEcritureMessage].humSdb = ((uint8_t)VMC.cuisineTropHumide << 4) | ((uint8_t)VMC.moisOKentreAirFrais << 3 ) | ( (uint8_t)VMC.heureOKentreAirFrais << 2) | ((uint8_t)VMC.entreAirChaudHiver << 1) | ((uint8_t)VMC.entreAirFraisEte) ;
		//lesMessages[pointeurEcritureMessage].etatVmc = ((uint8_t)VMC.getLeMode() << 2) | ((uint8_t)RELAIS.getEtatReelRelaisMarcheArret() << 1) | (uint8_t)RELAIS.getEtatRelaisVitesse();
lesMessages[pointeurEcritureMessage].etatVmc =((uint8_t)VMC.getLeMode() << 2) | ((uint8_t)RELAIS.getEtatReelRelaisMarcheArret() << 1) | ((uint8_t)RELAIS.getEtatRelaisVitesse());
		//DebugF("LeMode:"); Debugln((uint8_t)VMC.getLeMode());
		//DebugF("Vitesse:"); Debugln((uint8_t)RELAIS.getEtatRelaisVitesse());
		//DebugF("AM:"); Debugln((uint8_t)RELAIS.getEtatReelRelaisMarcheArret());
		//DebugF("etatVmc:"); Debugln(lesMessages[pointeurEcritureMessage].etatVmc);
		lesMessages[pointeurEcritureMessage].etatWifi = WIFI.getWifiUser() | (MYTINFO.getEtatWifi() << 1);
		pointeurEcritureMessage += 1;
		if (pointeurEcritureMessage >= NBMESSAGE)
			pointeurEcritureMessage = 0;
		if (comptMessage < NBMESSAGE)   //- 1  on continu a enregistrer,perte des plus anciens
			comptMessage += 1;          	//mais le nombre disponible n'augmente plus 
		else
		{
			pointeurLectureMessage +=1;
			if (pointeurLectureMessage >= NBMESSAGE)
				pointeurLectureMessage = 0;
		}
		if (premiersEnregistrement < 6)
			premiersEnregistrement++;				//on passe au compteur suivant
		clrCptSecPeriodeEnregistrement();
#ifdef 	WITHWIFINFO
	}
#endif
}
bool enregistrement::traiteCompteursTempo(ST_message *leMessage, uint8_t premEnr)
{
	char  valeurs[LONGMAXMOT];
		memset(valeurs, 0, sizeof(valeurs));
		TINFO.valueGet(&TableauTempoName[TEMPO_UTILISE::ETU_PTEC][0], &valeurs[0]);
		if (!((valeurs != NULL) && (valeurs[0] != '\0')))
		{
			erreur = ERREURS::E_TINFO;
			//DebuglnF("PTEC== NULL");
			return false;
		}
		//0:bleu nuit,1:blanc nuit,2:rouge nuit,3:bleu jour,4:blanc jour,5:rouge jour
		for (uint8_t HcHpCouleur = 0; HcHpCouleur < 6; HcHpCouleur++)
		{
			if (!strcmp(tableauPtec[HcHpCouleur], valeurs))
			{
				memset(valeurs, 0, sizeof(valeurs));
				if (premEnr > 5)   //cas normal
				{
					leMessage->etatTempo = HcHpCouleur | (HcHpCouleur << 3);  //3 bits lsb ptec  3 bits suivant ptec
					TINFO.valueGet(&TableauTempoName[HcHpCouleur][0], &valeurs[0]);
					if (!((valeurs != NULL) && (valeurs[0] != '\0')))
					{
						erreur = ERREURS::E_TINFO;
						//DebuglnF("CPT== NULL");
						return false;
					}
					leMessage->compteur = (unsigned int)atoi(valeurs);
				}
				else
				{
					leMessage->etatTempo = HcHpCouleur | (premEnr << 3); //3 bits lsb ptec 3 bits suivant Ne compteur
					TINFO.valueGet(&TableauTempoName[premEnr][0], &valeurs[0]);
					if (!((valeurs != NULL) && (valeurs[0] != '\0')))
					{
						erreur = ERREURS::E_TINFO;
						//DebugF("pb compteur: "); Debugln(premEnr);
						return false;
					}
					leMessage->compteur = (unsigned int)atoi(valeurs);
				}
				break;
			}
		}
	return true;
}
void enregistrement::traitementEmissionMessageTempoVMC()
{
	static bool premierMessage = true;
	if (premierMessage)             //pour la reemission,les pointeurs sont mis a jour au cycle suivant si acquitement
	{
		emetEnregistrementTempoVmc();
		premierMessage = false;
	}
	else if (dernierMessageEmisOK== ACQUITEMENT::RECUOK)  //dernier message emis valide
	{
		pointeurLectureMessage += nbMessageEmis;			//mise a jour des pointeurs
		if (pointeurLectureMessage >= NBMESSAGE)  
			pointeurLectureMessage = 0;      //pas de retour a 0 au milieu du paquet 
		comptMessage -= nbMessageEmis;
#ifdef  DEBUGENREGISTREMENT
//		DebugF("pointeurLectureMessage= ");Debugln(pointeurLectureMessage);
//		DebugF("comptMessage dans lecture= ");Debugln(comptMessage);

#endif
		nbMessageEmis = 0; 
		if (comptMessage > 0)
		{
			emetEnregistrementTempoVmc();	

		}
	}
	else if(dernierMessageEmisOK == ACQUITEMENT::RECUNOK)
	{
		if (comptMessage > 0)					//reemission du message 
		{
			emetEnregistrementTempoVmc();
#ifdef  DEBUGENREGISTREMENT
//			DebugF("reemission pointeurLectureMessage+=1= ");Debugln(pointeurLectureMessage);
//			DebugF("comptMessage apres - = ");Debugln(comptMessage);
#endif
		}
	}
		dernierMessageEmisOK = ACQUITEMENT::NONRECU;
	return;
}
void enregistrement::emetEnregistrementTempoVmc(void)
{
#ifdef  DEBUGENREGISTREMENT
//	DebuglnF("emetEnregistrement");
#endif
//#ifdef EMISSION_ENREGISTREMENT_VB
	//#define UNMES
	#ifdef UNMES
		nbMessageEmis = 1; //limite a 1 message
		union U_message U_leMessage;
		U_leMessage.leMessage = lesMessages[pointeurLectureMessage];
		//#ifdef EMISSION_ENREGISTREMENT_VB
		if (!send(CONFIGURATION.config.tempo.host_enr, (uint32)CONFIGURATION.config.tempo.portEnr, U_leMessage.leMessageChar,&memoport,udpEnr))
//			DebuglnF("pb udp.send message(vb)");
		//#else
		//	emission http
		//#endif
	#else
		//*******************************************************************************************************
		int limiteNbMaxMes = NBMESSAGE - pointeurLectureMessage;   //pas de retour a 0 au milieu du paquet
		if (comptMessage > NBMESSAGEEMISMAXUDP)
			nbMessageEmis = NBMESSAGEEMISMAXUDP;
		else
			nbMessageEmis = comptMessage;     // 

		if (nbMessageEmis > limiteNbMaxMes)
			nbMessageEmis = limiteNbMaxMes;
		#ifdef  DEBUGENREGISTREMENT
//			DebugF("nbMessageEmis UDP=");Debugln(nbMessageEmis);
		#endif
		int8_t longMessage = sizeof(ST_message);
		char *messageConcatene;
		messageConcatene =new char[longMessage*nbMessageEmis ];
		int16_t indice = 0;
		for (int mes = pointeurLectureMessage; mes < (pointeurLectureMessage + nbMessageEmis); mes++)
		{
			union U_message U_leMessage;
			U_leMessage.leMessage = lesMessages[mes];
			for (int octet = 0; octet < longMessage; octet++)
				{
				messageConcatene[indice*longMessage + octet] = U_leMessage.leMessageChar[octet];
				}
				indice += 1;
		}
		if (!send(CONFIGURATION.config.tempo.host_enr,(uint32) CONFIGURATION.config.tempo.portEnr, messageConcatene,&memoPort,udpEnr,nbMessageEmis))
//			DebuglnF("pb udp.send message(vb)");  
		delete[] messageConcatene;
	#endif
//#else
//	if (!WEBCLIENT.emetEmoncmsJsonVmc(lesMessages[pointeurLectureMessage]))
//		DebuglnF("ENR json:pb udp.send message (php)");
//#endif
	}
//bool enregistrement::send(IPAddress adressIP, uint32_t port, char * message, uint32_t * memPort, WiFiUDP udp, int16_t nbMessage)
bool enregistrement::send(const char* adressIP, uint32_t port, char * message, uint32_t * memPort,WiFiUDP udp, int16_t nbMessage)
{
	if (*memPort != port)
	{
		stop();
		udp.begin(port);
		*memPort = port;
	}
	int ret = -1;
	ret = udp.beginPacket(adressIP, port);  //adresse et port distant
	if (ret)
	{
		udp.write(message, sizeof(ST_message)*nbMessage);  //ou udp.write(buffer,longueur);  return le nombre de byte emis
		ret = udp.endPacket();
		if (ret)
			return true;
		else
			DebugF("pb udp.endPacket:");
	}
	else
	{
		DebugF("pb udp.beginPacket:");
	}
#ifdef  DEBUGENREGISTREMENT
	DebugF("adresseIP,Port:"); Debug(adresseIP);DebugF("-");Debugln(port);
#endif
	return false;
}
//testAcquitementMessage appele toutes les secondes
void enregistrement::testAcquitementMessage(void)
{
	//if(CONFIGURATION.config.tempo.acquitementEnr)
	//{
		char packetBuffer[255]; 
		int packetSize = udpEnr.parsePacket();
		if (packetSize)
		{
			int len = udpEnr.read(packetBuffer, 255);
			if (len>0)
			{
				if (packetBuffer[0]==49)    //on emet le suivant  49="1"
					dernierMessageEmisOK = ACQUITEMENT::RECUOK;
				else
					dernierMessageEmisOK = ACQUITEMENT::RECUNOK;
				//emission							//on emet le meme
//#ifdef  DEBUGENREGISTREMENT
				//DebugF("acquitement recu UDP= ");Debugln(packetBuffer[0]);
//#endif
			}
#ifdef  DEBUGENREGISTREMENT
			else
			{
				DebugF("acquitement recu UDP vide");Debugln(len);
			}
#endif
		}
}
int16_t  enregistrement::getComptMessage(void) const
{
	return comptMessage;
}
int16_t  enregistrement::getPtre(void) const
{
	return pointeurEcritureMessage;
}
int16_t  enregistrement::getPtrl(void) const
{
	return pointeurLectureMessage;
}

ST_message  enregistrement::getEnregistrementPourJson(void)
{

	static bool premierMessage = true;
	if (premierMessage)             //pour la reemission,les pointeurs sont mis a jour au cycle suivant si acquitement
	{
		premierMessage = false;
		dernierMessageEmisOK = ACQUITEMENT::NONRECU;
		return lesMessages[pointeurLectureMessage];
	}
	else if (dernierMessageEmisOK)  //dernier message emis valide
	{
		pointeurLectureMessage += nbMessageEmis;			//mise a jour des pointeurs
		if (pointeurLectureMessage >= NBMESSAGE)
			pointeurLectureMessage = 0;      //pas de retour a 0 au milieu du paquet 
		comptMessage -= nbMessageEmis;
#ifdef  DEBUGENREGISTREMENT
		DebugF("pointeurLectureMessage= "); Debugln(pointeurLectureMessage);
		DebugF("comptMessage dans lecture= "); Debugln(comptMessage);
#endif
		dernierMessageEmisOK = ACQUITEMENT::NONRECU;
		if (comptMessage > 0)
			return lesMessages[pointeurLectureMessage];
	}
	else
	{
		if (comptMessage > 0)					//reemission du message 
		{
			return lesMessages[pointeurLectureMessage];
#ifdef  DEBUGENREGISTREMENT
			DebugF("reemission pointeurLectureMessage+=1= "); Debugln(pointeurLectureMessage);
			DebugF("comptMessage apres - = "); Debugln(comptMessage);
#endif
		}
	}
	ST_message leMessage;
	leMessage.heure1970 = 0;
	return leMessage;
}
ST_message  enregistrement::getEnregistrementTempsReel(void) 
{
	ST_message leMessage;
	//*******************************************************************
	traiteCompteursTempo(&leMessage, 6);
	//****************************************************
	leMessage.heure1970 = Clock.getTimeSeconds();
	char  valeurs[] = { "\0" };
	leMessage.iInstMax = atoi(TINFO.valueGet(&TableauTempoName[TEMPO_UTILISE::ETU_IINST][0], &valeurs[0]));   //CAPTEURIINST.getMaxi();
	leMessage.dureeMax = this->dureeMax;
	leMessage.tempExt = TEMPEXT.getMoyennePeriode();
	leMessage.tempCuis = DHTCUISINE_T.getMoyennePeriode();
	leMessage.tempSdb = DHTSDB.DHT_T.getMoyennePeriode();
	leMessage.humCuis = DHTCUISINE_H.getMoyennePeriodeLsb();//0 e 100--->7 bits
	leMessage.humSdb = DHTSDB.DHT_H.getMoyennePeriodeLsb();
	leMessage.etatVmc = (VMC.getLeMode() << 2) | (RELAIS.getEtatReelRelaisMarcheArret() << 1) | RELAIS.getEtatRelaisVitesse();
	leMessage.etatWifi = WIFI.getWifiUser();
	return leMessage;
}
void enregistrement::setdernierMessageEmisOK()
{
	dernierMessageEmisOK = ACQUITEMENT::RECUOK;
}
uint8_t enregistrement::getEtResetErreur(void)
{
	uint8_t temp = erreur;
	erreur = 0;
	return temp;
}
void enregistrement::forceEnregistrementSurMarche(void)
{
	forceEnregistrement =true;
}
