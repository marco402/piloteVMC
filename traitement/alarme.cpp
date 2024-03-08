// **********************************************************************************
// Programme traitement ESP8266 Teleinfo Traitement du message d'alarme
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// Written by Marc Prieur (https://marco40github.wixsite.com/website))
//
// History : V1.00 2022-09-05 - First release
//
// All text above must be included in any redistribution.
//
// **********************************************************************************
/*principe:
  cote alarme:
    -relais reed ferme au repos(porte ouverte)actuellement 0:porte fermee
    -poussoir arret alarme 0 pour arret.
    -a l'ouverture de la porte,10 secondes pour appuyer sur un poussoir
    -l'alarme se reactive a la fermeture de la porte
    -reemission du dernier message toutes les heures
    -traitement acquitement reemission 3 fois. 
    codes messages:  
      DOOR_OPEN_WITH_ALARME
      DOOR_OPEN_WITHOUT_ALARME
      DOOR_CLOSE_WITH_ALARME
      DOOR_CLOSE_WITHOUT_ALARME
      ALARME_DECLENCHEE
  
  cote traitement tempo
    -test de reception du message toutes les cycles
      -sans reception on reste sur la derniere valeur
      -si DOOR_CLOSE_WITH_ALARME                                  ->couleur verte
      -si DOOR_CLOSE_WITHOUT_ALARME                               ->couleur blanc  devrait pas arriver
      -si DOOR_OPEN_WITH_ALARME                                   ->couleur jaune  temporaire max 10 secondes
      -si DOOR_OPEN_WITHOUT_ALARME                                ->couleur bleu->porte ouverte avec desactivation de l'alarme
      -si pas de message recu depuis > 60 secondes:code HEARTBEAT ->couleur orange
      -si ALARME_DECLENCHEE                                       ->couleur rouge
      -arret de l'alarme en passant par arret de la vmc si deja sur arret passer par lent puis arret
      -reemission des acquitements

  cote affichage
      -juste affichage des couleurs
      
*/
//***********************************************************************************
#include <WiFiUdp.h>
#include "config.h"
#include "alarme.h"
#include "mySyslog.h"
myAlarme::myAlarme()
{}
void ICACHE_FLASH_ATTR myAlarme::init(INDICEALARMES indice)
{
  indiceAlarme=indice;
  if(indiceAlarme==INDICEALARMES::GARAGE)
  {
   DebugF("port garage: ");
   etatAlarme = enumCouleursALARME::ST7735_ORANGE;
  }
  else
  {
   DebugF("port portail: ");
   etatAlarme = enumCouleursALARME::ST7735_ORANGE; 
  }

  Debugln(CONFIGURATION.config.tempo.portEnr+indiceAlarme);
	udpAlarme.begin(CONFIGURATION.config.tempo.portEnr+indiceAlarme);
    memoPort = CONFIGURATION.config.tempo.portEnr+indiceAlarme;
}
void ICACHE_FLASH_ATTR myAlarme::stop()
{
	udpAlarme.stop();
}
///
void myAlarme::testReceptionAlarme(void)
{
  char packetBuffer[100];
	byte packetBufferAcquitement[3];
	packetBufferAcquitement[0] = (byte) CODES_ALARME::HEAD_MESSAGE;
	packetBufferAcquitement[1] = (byte) CODES_ALARME::ACQUITTEMENT;
 
	int packetSize = udpAlarme.parsePacket();
	if(razAlarme)
    {
    	if (packetSize)
    	{
    		int len = udpAlarme.read(packetBuffer, 255);
    		if (len > 1)
    		{
        DebuglnF("reception udp alarme,len>1: ");
    			if (packetBuffer[0] == (char)CODES_ALARME::HEAD_MESSAGE)
    			{
    ////      if ((char)packetBuffer[1]>=(char)CODES_ALARME::INCHANGE)
    ////        etatAlarme = (char)CODES_ALARME::NO_RECEPT;
    ////      else
    ////      {
    
            etatAlarme = (char)packetBuffer[1];
            if (etatAlarme==CODES_ALARME::ALARME_DECLENCHEE)
              razAlarme=false;        //attente passage par arret pour remettre a true
                      
            if(indiceAlarme==INDICEALARMES::GARAGE)
                DebugF("reception udp garage: ");
            else
                DebugF("reception udp portail: ");           
            Debugln(etatAlarme);  
            packetBufferAcquitement[2] = etatAlarme;  //reemission du code recu
    
          //emission acquittement        
            if (!send(udpAlarme.remoteIP(),(uint32) CONFIGURATION.config.tempo.portEnr + indiceAlarme, packetBufferAcquitement,&memoPort))
               DebuglnF("pb udp send acq: ");
            else
               DebugF("send heartbeat: ");Debugln(etatAlarme);           
    ////       } 
    			receptLastMessage = 0; 
			  }
			}
		}
	}
	if (receptLastMessage > 60)
	{
    etatAlarme = (char)CODES_ALARME::HEARTBEAT;
//    packetBufferAcquitement[2] =etatAlarme;
		receptLastMessage=0;
  
//   if (!send(udpAlarme.remoteIP(),(uint32) CONFIGURATION.config.tempo.portEnr + indiceAlarme, packetBufferAcquitement,&memoPort))
//      DebugF("pb udp send heartbeat: ");
//   else
//      DebugF("send heartbeat: ");
   Debugln(etatAlarme);
	}
}
 char myAlarme::getEtatAlarme(void)
{
	return etatAlarme;
}
void  myAlarme::setRazAlarme(void)
{
  razAlarme=true;
}

bool myAlarme::send(IPAddress adresseIP, uint32_t port, byte * message, uint32_t * memPort)
{
	if (*memPort != port)
	{
		stop();
		udpAlarme.begin(port);
		*memPort = port;
	}
 char  toprint[20];
  sprintf(toprint, "%d.%d.%d.%d", adresseIP[0], adresseIP[1], adresseIP[2], adresseIP[3]);
  DebugF("adresseIP,Port: "); Debug(toprint); DebugF("-"); Debugln(port);
if (adresseIP[0]>0)
  {
    int ret = -1;
  	ret = udpAlarme.beginPacket(adresseIP, port);  //adresse et port distant
  	if (ret)
  	{
  		udpAlarme.write(message, sizeof(message));  //4 octet sur wireshark  le 4eim=3f ?                return le nombre de byte emis  
  		ret = udpAlarme.endPacket();
  		if (ret)
      {
  			return true;
      }
  		else
  			DebuglnF("pb udp.endPacket:");
  	}
  	else
  	{
  		DebuglnF("pb udp.beginPacket:");
  	}
  }
  else
     return true;
	return false;
}
