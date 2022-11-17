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
		-relais reed ferme au repos(porte ouverte)
		-emission d'un message etat de la porte au demarrage.
		-emission d'un message etat de la porte 1 fois par jour entre 50000 et 80000 secondes(heartbeat->porte ouverte:code DOOR_OPEN_WITH_ALARME ou DOOR_OPEN_WITHOUT_ALARME ou porte fermee:code DOOR_CLOSE)
		-emission d'un message sur changement d'etat du contact(porte ouverte:code DOOR_OPEN_WITH_ALARME ou DOOR_OPEN_WITHOUT_ALARME ou porte fermee:code DOOR_CLOSE)
		-si DOOR_OPEN_WITH_ALARME envoi d'un mail et allumage lampe
		-a l'ouverture de la porte,10 secondes pour appuyer sur un poussoir->emission code DOOR_OPEN_WITHOUT_ALARME
		-l'alarme se reactive a la fermeture de la porte
		-pour tous les messages:attente d'un acquitement, reemission si pas recu
		-gerer les rebonds du relais
	cote traitement tempo / affichage
		-test de reception du message toutes les cycles
			-sans reception on reste sur la derniere valeur
			-acquitement du message sur reception
			-si pas de message recu depuis 90000secondes:code HEARTBEAT vers affichage->couleur orange
			-si reception code DOOR_CLOSE, code DOOR_CLOSE vers affichage->couleur verte
			-si reception code DOOR_OPEN_WITH_ALARME, code DOOR_OPEN_WITH_ALARME vers affichage->couleur rouge->porte ouverte sans desactivation de l'alarme->bip sonore:
			-si DOOR_OPEN_WITH_ALARME allumage lampe
			-si reception code DOOR_OPEN_WITHOUT_ALARME, code DOOR_OPEN_WITHOUT_ALARME vers affichage->couleur bleu->porte ouverte avec desactivation de l'alarme->pas de bip sonore::
      
			-arret de l'alarme en passant par arret de la vmc.
*/
//***********************************************************************************
#include <WiFiUdp.h>
#include "config.h"
#include "alarme.h"
#include "mySyslog.h"
myAlarme::myAlarme()
{}
void ICACHE_FLASH_ATTR myAlarme::init(int indice)
{
  indiceAlarme=indice;
  if(indiceAlarme==1)
  {
   DebuglnF("port garage");
   etatAlarme = enumCouleursALARME::ST7735_GREEN;
  }
  else
  {
   DebuglnF("port portail");
   etatAlarme = enumCouleursALARME::ST7735_BLUE; 
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
	char packetBufferAcquitement[2];
	packetBufferAcquitement[0] = ( char)CODES_ALARME::HEAD_MESSAGE;
	packetBufferAcquitement[1] = ( char)CODES_ALARME::AQUITEMENT;
 
	int packetSize = udpAlarme.parsePacket();
	receptLastMessage += 1;
	if (packetSize)
	{
		int len = udpAlarme.read(packetBuffer, 255);
		if (len > 0)
		{
			if (packetBuffer[0] == (char)CODES_ALARME::HEAD_MESSAGE)
			{
      if ((char)packetBuffer[1]>=(char)CODES_ALARME::FIN)
        etatAlarme = (char)CODES_ALARME::NO_RECEPT;
      else
      {
        etatAlarme = (char)packetBuffer[1];
        if(indiceAlarme==1)
            DebugF("reception udp garage: ");
        else
            DebugF("reception udp portail");           
        Debugln(etatAlarme);  
        packetBufferAcquitement[2] = packetBuffer[1];  //reemission du code recu
      //emission acquittement        
        if (!send(udpAlarme.remoteIP(),(uint32) CONFIGURATION.config.tempo.portEnr + indiceAlarme, packetBufferAcquitement,&memoPort))
           DebugF("pb udp send: "); Debugln(indiceAlarme);       
      } 
			receptLastMessage = 0; 
			}
		}
	}
	if (receptLastMessage > 60)
	{
		receptLastMessage-=1;   //pour pas de debordement
		etatAlarme = (char)CODES_ALARME::HEARTBEAT;
	}
}
 char myAlarme::getEtatAlarme(void)
{
	return etatAlarme;
}

bool myAlarme::send(IPAddress adresseIP, uint32_t port, char * message, uint32_t * memPort)
{
	if (*memPort != port)
	{
		stop();
		udpAlarme.begin(port);
		*memPort = port;
	}
 char  toprint[20];
  sprintf(toprint, "%d.%d.%d.%d", adresseIP[0], adresseIP[1], adresseIP[2], adresseIP[3]);
  DebugF("adresseIP,Port:"); Debug(toprint); DebugF("-"); Debugln(port);
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
			DebugF("pb udp.endPacket:");
	}
	else
	{
		DebugF("pb udp.beginPacket:");
	}
	

	return false;
}
