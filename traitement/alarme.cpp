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
		-emission d'un message au demarrage
		-emission d'un message 1 fois par jour a midi(heartbeat->porte ouverte:code DOOR_OPEN_WITH_ALARME ou DOOR_OPEN_WITHOUT_ALARME ou porte fermee:code DOOR_CLOSE)
		-emission d'un message sur changement d'etat du contact(porte ouverte:code DOOR_OPEN_WITH_ALARME ou DOOR_OPEN_WITHOUT_ALARME ou porte fermee:code DOOR_CLOSE)
		-si DOOR_OPEN_WITH_ALARME envoi d'un mail et allumage lampe
		-attente d'un acquitement, reemission si pas recu
		-a l'ouverture de la porte,10 secondes pour appuyer sur un poussoir->emission code DOOR_OPEN_WITHOUT_ALARME
		-l'alarme se reactive a la fermeture de la porte
		-pour tous les messages:attente d'un acquitement, reemission si pas recu
		-gerer les rebonds du relais
	cote traitement tempo / affichage
		-test de reception du message toutes les secondes
			-sans reception on reste sur la derniere valeur
			-acquitement du message sur reception
			-si pas de message recu vers 12 heure 30,code HEARTBEAT vers affichage->couleur orange
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
void ICACHE_FLASH_ATTR myAlarme::init()
{
	udpAlarme.begin(CONFIGURATION.config.tempo.portEnr+1);
    memoPort = CONFIGURATION.config.tempo.portEnr+1;
}
void ICACHE_FLASH_ATTR myAlarme::stop()
{
	udpAlarme.stop();
}
///
void myAlarme::testReceptionAlarme(void)
{
	return;
	char packetBuffer[255];
	char packetBufferAcquitement[2];
	packetBufferAcquitement[0] = CODES_ALARME::HEAD_MESSAGE;
	packetBufferAcquitement[1] = CODES_ALARME::AQUITEMENT;
	int packetSize = udpAlarme.parsePacket();
	receptLastMessage += 1;
	if (packetSize)
	{
		int len = udpAlarme.read(packetBuffer, 255);
		if (len > 0)
		{
			if (packetBuffer[0] == CODES_ALARME::HEAD_MESSAGE)
			{
				etatAlarmeGarage = (uint8_t)packetBuffer[1];
				receptLastMessage = 0;
				//emission acquittement
        if (!send(udpAlarme.remoteIP(),(uint32) CONFIGURATION.config.tempo.portEnr+1, packetBufferAcquitement,&memoPort,udpAlarme,1))
					DebuglnF("pb udp.send message acquitement");
			}
		}
	}
	if (receptLastMessage >= 90000)
	{
		receptLastMessage-=1;
		etatAlarmeGarage = (uint8_t)CODES_ALARME::HEARTBEAT;
	}
}
uint8_t myAlarme::getEtatAlarmeGarage(void)
{
	return etatAlarmeGarage;
}
uint8_t myAlarme::getEtatAlarmePortail(void)
{
	return etatAlarmePortail;
}
bool myAlarme::send(IPAddress adresseIP, uint32_t port, char * message, uint32_t * memPort, WiFiUDP udp, int16_t nbMessage)
{
	if (*memPort != port)
	{
		stop();
		udp.begin(port);
		*memPort = port;
	}
	int ret = -1;
	ret = udp.beginPacket(adresseIP, port);  //adresse et port distant
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
	DebugF("adresseIP,Port:"); Debug(adresseIP); DebugF("-"); Debugln(port);
#endif
	return false;
}
