// **********************************************************************************
// ESP8266 Pilotage VMC et TEMPO Traitement du message pour  enregistrement
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
//06/06/2018 pass� NBMESSAGE de 100 � 300     pour 5 minutes 24 heures:288

//avec 300 et vmc
//Program traitement_tempo size: 372�588 bytes (used 36% of a 1�044�464 byte maximum) (121,39 secs)
//Minimum Memory Usage : 63260 bytes(77 % of a 81920 byte maximum)
//avec 500
//Program traitement_tempo size : 372�588 bytes(used 36 % of a 1�044�464 byte maximum) (122, 30 secs)
//Minimum Memory Usage : 75660 bytes(92 % of a 81920 byte maximum)

//en version structure num�rique et 200 messages vmc+tempo:
//Program traitement_tempo size: 371�848 bytes (used 36% of a 1�044�464 byte maximum) (126,71 secs)
//Minimum Memory Usage : 49944 bytes(61 % of a 81920 byte maximum)


#ifndef ENREGISTREMENT_H
#define ENREGISTREMENT_H

#include <Arduino.h>
#include <WiFiUdp.h>
#define NBMESSAGE 200				//ATTENTION,pas confirm� mais l'OTA ne fonctionnait plus:Fatal exception 29(StoreProhibitedCause) en mettant 300.
//le port s�rie ne fonctionnait plus sur la carte nodemcu base 1.0, ok sans la base et a nouveau ok avec la base en rechargeant la version avec 200??? � suivre.
//� 200:7.43kb free ram sur page web

#define NBMESSAGEEMISMAXUDP 10     //a 10 le temps de cycle d�passe la seconde 1086,1300(c'�tait pas �a � suivre) 
//sans mail:300 (25 heures et 43(Long mes) pas de plantage mais pertes de messages et d�calage(2/12/18:delta 18963->perte:63 messages(periode d'enr:300)						50 pb 100 pb mail...		
//ensuite lecture message:a 9h50 s'arrete a 3h36--->6h14-->374mn/5(300sec)=decalage:				 74 messages

//4/12/18 essai avec NBMEMaxDureeSSAGE=200--->16 heures
enum ACQUITEMENT { NONRECU = 0, RECUOK,RECUNOK };
struct ST_message				//24 bytes
{
	uint32_t heure1970;				//4
	//partie tempo
	uint32_t compteur;			//4
	uint16_t iInstMax;			//2
	uint16_t dureeMax;			//2
	uint8_t etatTempo;			//1
	//partie esp
	uint8_t etatWifi;			//1
	//partie vmc
	int16_t tempExt;			//2
	int16_t tempCuis;			//2
	int16_t tempSdb;			//2
	uint8_t humCuis;			//1
	uint8_t humSdb;				//1
	uint8_t reserve;			//1
	uint8_t etatVmc;			//1
};
union U_message
{
	ST_message leMessage;
	char leMessageChar[sizeof(ST_message)];
};

class enregistrement
{
public:

	enregistrement(void);
	void TRAITEENREGISTREMENT( boolean  nouvelleTrame, boolean  cgtCompteur, uint16_t dureeMax);
	void testAcquitementMessage(void);
	void init(void);
	void stop(void);
	void clrCptSecPeriodeEnregistrement(void);
	void setCptSecPeriodeEnregistrement(void);
	uint16_t getpremiersEnregistrement(void);
	void clrPremierEnregistrement(void);
	int16_t  getComptMessage(void) const;
	int16_t  getPtre(void) const;
	int16_t  getPtrl(void) const;
	void traitementEmissionMessageTempoVMC(void);
	ST_message  getEnregistrementTempsReel(void) ;
	ST_message getEnregistrementPourJson(void) ;
	void setdernierMessageEmisOK();
	uint8_t getEtResetErreur(void);
	void forceEnregistrementSurMarche(void);
private:
	WiFiUDP udpEnr;
  //IPAddress adressIP;
	void incCptSecPeriodeEnregistrement(void);
	void enregistreMessageTempoVmc();
  bool send(const char* adressIP, uint32_t port, char * message, uint32_t * memPort, WiFiUDP udp, int16_t nbMessageEmis);  
  //bool send(IPAddress adressIP, uint32_t port, char * message, uint32_t * memPort, WiFiUDP udp, int16_t nbMessageEmis);
	bool traiteCompteursTempo(ST_message *leMessage, uint8_t premiersEnregistrement);
	void emetEnregistrementTempoVmc(void);
	ST_message lesMessages[NBMESSAGE];
	int16_t  comptMessage = 0;  //public pour test
	int16_t pointeurEcritureMessage = 0;
	int16_t pointeurLectureMessage= 0;
	ACQUITEMENT dernierMessageEmisOK = ACQUITEMENT::NONRECU;
	int16_t nbMessageEmis = 0;
	uint16_t cptSecPeriodeEnregistrement = 0;
	uint8_t premiersEnregistrement = 0;
	int16_t dureeMax = 0;
	uint8_t erreur=0;
	boolean forceEnregistrement = false;
	uint32_t memoPort = 0;
};
extern enregistrement ENREGISTREMENT;

#endif
