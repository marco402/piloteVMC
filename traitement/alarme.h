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
//06/06/2018 passe NBMESSAGE de 100 e 300     pour 5 minutes 24 heures:288

//avec 300 et vmc
//Program traitement_tempo size: 372e588 bytes (used 36% of a 1e044e464 byte maximum) (121,39 secs)
//Minimum Memory Usage : 63260 bytes(77 % of a 81920 byte maximum)
//avec 500
//Program traitement_tempo size : 372e588 bytes(used 36 % of a 1e044e464 byte maximum) (122, 30 secs)
//Minimum Memory Usage : 75660 bytes(92 % of a 81920 byte maximum)

//en version structure numerique et 200 messages vmc+tempo:
//Program traitement_tempo size: 371e848 bytes (used 36% of a 1e044e464 byte maximum) (126,71 secs)
//Minimum Memory Usage : 49944 bytes(61 % of a 81920 byte maximum)


#ifndef ALARME_H
#define ALARME_H
#include <Arduino.h>
#include "Wifinfo.h"
#include <WiFiUdp.h>
#include "enregistrement.h"
#define CFG_TEMPO_HOST_SIZE    32

enum INDICEALARMES {GARAGE=1,PORTAIL=2  };
enum enumCouleursALARME {ST7735_WHITE=0, ST7735_RED, ST7735_GREEN,ST7735_BLUE,ST7735_ORANGE};
enum CODES_ALARME {  
  PREMIER = 0,
  DOOR_OPEN_WITH_ALARME,           //jaune transitoire maxi NBCYCLECONTACTSTOPALARME
  DOOR_OPEN_WITHOUT_ALARME,        //bleu
  DOOR_CLOSE_WITH_ALARME,          //vert
  DOOR_CLOSE_WITHOUT_ALARME,       //blanc
  HEARTBEAT,                       //orange
  ALARME_DECLENCHEE,               //rouge  
  HEAD_MESSAGE,
  START,
  ACQUITTEMENT,
  LEDERNIER};
class myAlarme
{
public:
	myAlarme(void);
	void testReceptionAlarme(void);
	void init(INDICEALARMES indice);
	void stop(void);
	char getEtatAlarme(void);
  void setRazAlarme(void);
private:
  INDICEALARMES indiceAlarme=INDICEALARMES::GARAGE;
	WiFiUDP udpAlarme;
	bool send(IPAddress adressIP, uint32_t port, byte * message, uint32_t * memPort);
	char etatAlarme;
  bool razAlarme=true;
	uint32_t receptLastMessage = 0;
  uint32_t memoPort = 0;
  char  host_alarme[CFG_TEMPO_HOST_SIZE + 1]; //adresse IP
};
extern myAlarme MYALARMEGARAGE;
extern myAlarme MYALARMEPORTAIL;
#endif
