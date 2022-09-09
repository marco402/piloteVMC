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


#ifndef ALARME_H
#define ALARME_H
#include <Arduino.h>
#include "Wifinfo.h"
#include <WiFiUdp.h>
#include "enregistrement.h"  //for send a voir
#define CFG_TEMPO_HOST_SIZE    32
enum CODES_ALARME { DOOR_OPEN_WITH_ALARME = 1, DOOR_CLOSE, DOOR_OPEN_WITHOUT_ALARME ,HEARTBEAT,HEAD_MESSAGE,AQUITEMENT};
  //                       ROUGE                   VERT              BLEU                ORANGE
class myAlarme
{
public:
	myAlarme(void);
	void testReceptionAlarme(void);
	void init(void);
	void stop(void);
	uint8_t getEtatAlarmeGarage(void);
private:
	WiFiUDP udpAlarme;
	bool send(IPAddress adressIP, uint32_t port, char * message, uint32_t * memPort, WiFiUDP udp, int16_t nbMessageEmis);
	uint8_t etatAlarmeGarage = 0;
	uint32_t receptLastMessage = 0;
  uint32_t memoPort = 0;
  char  host_alarme[CFG_TEMPO_HOST_SIZE + 1]; //adresse IP
};
extern myAlarme MYALARME;
#endif
