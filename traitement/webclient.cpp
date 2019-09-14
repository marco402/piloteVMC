// **********************************************************************************
// ESP8266 Teleinfo WEB Client, web client functions
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// For any explanation about teleinfo ou use, see my blog
// http://hallard.me/category/tinfo
//
// This program works with the Wifinfo board
// see schematic here https://github.com/hallard/teleinfo/tree/master/Wifinfo
//
// Written by Charles-Henri Hallard (http://hallard.me)
//
// History : V1.00 2015-12-04 - First release
//
// All text above must be included in any redistribution.
//
// Modifié par marc Prieur 2019
//		-V2.0.0:intégré le code dans la classe webClient webClient.cpp  webClient.h
//		-V2.0.2:mise en place TAILLEBUFEMONCMS modidié les messages de httpPost
//             :dans build_emoncms_json test si la liste est vide sinon plantage si on a jamais recu de trame.
//		-V2.0.3 modification pour affichage DEMAIN sur emoncms passé de chaine à numériqueà préciser
//				-ajout des champs teleinfo standard,vérification du nom des champs transférés dans LibTeleinfo
//				-passé DEMAIN en numérique dans build_emoncms_json
// Using library ESP8266HTTPClient version 1.1
//
//
// ************************************************************************************************************
#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include "Wifinfo.h"
#include "mySyslog.h"
#include "LibTeleinfo.h"
#include "config.h"
#include "simuTempo.h"
#include "enregistrement.h"
#include "webclient.h"

webClient::webClient(boolean modeLinkyHistorique)
{
	this->modeLinkyHistorique = modeLinkyHistorique;
	//http.setTimeout(200);   //1500
}


/* ======================================================================
Function: httpPost
Purpose : Do a http post
Input   : hostname
          port
          url
Output  : true if received 200 OK
Comments: -
====================================================================== */
//boolean webClient::httpPost(char * host, uint16_t port, char * url)
//{
//	//HTTPClient http;
//	//bool ret = false;
//
//	//unsigned long start = millis();
//
//	// configure traged server and url
//	http.setTimeout(200);   //1500
//	//http.begin(host, port, url);
//	//http.begin("http://192.168.1.69:80/emoncms/input/post.json?node=99&apikey=cc91f1942df2fb621e6901cb46c99429&json={TEST:105900}");
//	http.begin("http://192.168.1.69:80/emoncms/input/post?node=99&apikey=cc91f1942df2fb621e6901cb46c99429&json={ADCO:039801172264,OPTARIF:4,ISOUSC:45,BBRHCJB:029252680,BBRHPJB:063174800,BBRHCJW:005079009,BBRHPJW:010795471,BBRHCJR:000844643,BBRHPJR:002473349,PTEC:9,DEMAIN:10,IINST:001,IMAX:029,HHPHC:89,MOTDETAT:000000}");
//		ESP.wdtFeed();  //Force software watchdog to restart from 0  ajout marc
//
//	String errorMes;
//	char  buffer[TAILLEBUFEMONCMS];  //marc 132-->400
//	if (strlen(url) + CFG_EMON_HOST_SIZE + 27 < TAILLEBUFEMONCMS)
//	{
//		sprintf(buffer, "http%s://%s:%d%s => ", port == 443 ? "s" : "", host, port, url);
//		Debug(buffer);
//	}
//	else {
//		errorMes = "PB:TAILLEBUFEMONCMS trop petite";
//	}
//		//?http.addHeader("Content-Type", "text/plain");
//		// start connection and send HTTP header
//
//	transfertHttpEnCours = true;
//	attenteHttp = millis() + 1000;
//	Debugln(errorMes);
//	//this->httpGetPost();
//	return true;
//}

//version avec httpGetPost et timeout 300 et message en dur:
			//apache bloqué:344ms
			//apache ok avec ou sans emoncms moyenne 75ms max 103ms temps que du get
//version d'origine timeout 200 et message en dur 
			//apache bloqué:447ms
			//apache ok avec ou sans emoncms moyenne 300ms temps qui englobe tout 


boolean webClient::httpPost(char * host, uint16_t port, char * url)
{
	HTTPClient http;
	bool ret = false;

	//unsigned long start = millis();

	// configure traged server and url
	http.setTimeout(200);   //1500
	http.begin(host, port, url);
	//exemple ok
	//http.begin("http://192.168.1.69:80/emoncms/input/post?node=99&apikey=cc91f1942df2fb621e6901cb46c99429&json={ADCO:039801172264,OPTARIF:4,ISOUSC:45,BBRHCJB:029252680,BBRHPJB:063174800,BBRHCJW:005079009,BBRHPJW:010795471,BBRHCJR:000844643,BBRHPJR:002473349,PTEC:9,DEMAIN:10,IINST:001,IMAX:029,HHPHC:89,MOTDETAT:000000}");

  
	String errorMes;
	  //char  buffer[TAILLEBUFEMONCMS];  //marc 132-->400
	  //plantage  uc, message trop long pour syslog? a voir
	  //if (strlen(url) + CFG_EMON_HOST_SIZE + 27 < TAILLEBUFEMONCMS)
	  //{
		 // sprintf(buffer, "http%s://%s:%d%s => ", port == 443 ? "s" : "", host, port, url);
		 // Debugln(buffer);
	  //}
	  //else {
		 // errorMes = "PB:TAILLEBUFEMONCMS trop petite";
	  //}

		//?http.addHeader("Content-Type", "text/plain");
		// start connection and send HTTP header
		//ESP.wdtFeed();  //Force software watchdog to restart from 0  ajout marc

		int httpCode = http.GET();
		if (httpCode) {
			// HTTP header has been send and Server response header has been handled
			//errorMes = "OK:httpCode:" + httpCode;
			// file found at server
				String payload = http.getString();
				if (httpCode == 200) {
					errorMes = "OK:payload:" + payload;
					ret = true;
				}
				else
					errorMes = "PB:httpCode(!200):" + String(httpCode);  //+ "  payload:" + payload;    //il faudrait passer le buffer à 500 ou + voir avec wireshark
		}
		else {
		  errorMes = "PB:httpCode:(GET)" + String(httpCode);
		}
	
	Debug(errorMes);

	//sprintf(buffer, " in %ld ms\r\n", millis() - start);
	//	Debugln(buffer);
	http.end();		//marc
	return ret;
}


//void webClient::httpGetPost()
//{
	///*if (transfertHttpEnCours && (millis()>attenteHttp))
	//{*/
	//bool ret = false;
	//unsigned long start = millis();
	//String errorMes;
	//int httpCode = this->http.GET();
	//if (httpCode) {
	//	// HTTP header has been send and Server response header has been handled
	//	//errorMes = "OK:httpCode:" + httpCode;
	//	// file found at server
	//	String payload = http.getString();
	//	if (httpCode == 200) {
	//		errorMes = "OK:payload:" + payload;
	//		ret = true;
	//	}
	//	else
	//		errorMes = "PB:httpCode(!200):" + String(httpCode);  //+ "  payload:" + payload;    //il faudrait passer le buffer à 500 ou + voir avec wireshark
	//}
	//else {
	//	errorMes = "PB:httpCode:(GET)" + String(httpCode);
	//}
	//if(!ret)
	//	Debugln(errorMes);
	//char  buffer[TAILLEBUFEMONCMS];  //marc 132-->400
	//sprintf(buffer, " in %ld ms\r\n", millis() - start);
	//Debugln(buffer);
	//http.end();
	//transfertHttpEnCours = false;
	////}
	//return ;
//}

/* ======================================================================
Function: build_emoncms_json string (usable by webserver.cpp)
Purpose : construct the json part of emoncms url
Input   : -
Output  : String if some Teleinfo data available
Comments: -
====================================================================== */
String webClient::build_emoncms_json(void)
{

	boolean first_item = true;
	ESP.wdtFeed();  //Force software watchdog to restart from 0  ajout marc
	String url = "{";

	ValueList * me = TINFO.getList();
	if (me) {
		// Loop thru the node
		while (me->next) {
			if (!first_item)
				// go to next node
				me = me->next;
			//marc
			else
				if (me->free) {
					//1st item is free : empty list !
					Debugln("Teleinfo list is empty !");
					break;
				}
			//fin marc
			if (!me->free) {


				// On first item, do not add , separator
				if (first_item)
					first_item = false;
				else
					url += ",";

				url += me->name;
				url += ":";

				// EMONCMS ne sait traiter que des valeurs numériques, donc ici il faut faire une 
				// table de mappage, tout à fait arbitraire, mais c"est celle-ci dont je me sers 
				// depuis mes débuts avec la téléinfo
				if (!strcmp(me->name, "OPTARIF")) {
					// L'option tarifaire choisie (Groupe "OPTARIF") est codée sur 4 caractères alphanumériques 
					/* J'ai pris un nombre arbitraire codé dans l'ordre ci-dessous
					je mets le 4eme char à 0, trop de possibilités
					BASE => Option Base.
					HC.. => Option Heures Creuses.
					EJP. => Option EJP.
					BBRx => Option Tempo
					*/
					char * p = me->value;

					if (*p == 'B'&&*(p + 1) == 'A'&&*(p + 2) == 'S') url += "1";
					else if (*p == 'H'&&*(p + 1) == 'C'&&*(p + 2) == '.') url += "2";
					else if (*p == 'E'&&*(p + 1) == 'J'&&*(p + 2) == 'P') url += "3";
					else if (*p == 'B'&&*(p + 1) == 'B'&&*(p + 2) == 'R') url += "4";
					else url += "0";
				}
				else if (!strcmp(me->name, "HHPHC")) {
					// L'horaire heures pleines/heures creuses (Groupe "HHPHC") est codé par un caractère A à Y 
					// J'ai choisi de prendre son code ASCII
					int code = *me->value;
					url += String(code);
				}
				else if (!strcmp(me->name, "PTEC")) {
					// La période tarifaire en cours (Groupe "PTEC"), est codée sur 4 caractères 
					/* J'ai pris un nombre arbitraire codé dans l'ordre ci-dessous
					TH.. => Toutes les Heures.
					HC.. => Heures Creuses.
					HP.. => Heures Pleines.
					HN.. => Heures Normales.
					PM.. => Heures de Pointe Mobile.
					HCJB => Heures Creuses Jours Bleus.
					HCJW => Heures Creuses Jours Blancs (White).
					HCJR => Heures Creuses Jours Rouges.
					HPJB => Heures Pleines Jours Bleus.
					HPJW => Heures Pleines Jours Blancs (White).
					HPJR => Heures Pleines Jours Rouges.
					*/
					if (!strcmp(me->value, "TH..")) url += "1";
					else if (!strcmp(me->value, "HC..")) url += "2";
					else if (!strcmp(me->value, "HP..")) url += "3";
					else if (!strcmp(me->value, "HN..")) url += "4";
					else if (!strcmp(me->value, "PM..")) url += "5";
					else if (!strcmp(me->value, "HCJB")) url += "6";
					else if (!strcmp(me->value, "HCJW")) url += "7";
					else if (!strcmp(me->value, "HCJR")) url += "8";
					else if (!strcmp(me->value, "HPJB")) url += "9";
					else if (!strcmp(me->value, "HPJW")) url += "10";
					else if (!strcmp(me->value, "HPJR")) url += "11";
					else url += "0";
				}
				//ajout pour affichage sur emoncms à préciser numérique sinon chaine
				//on pourrait supprimer demain, peu d'interet pour emoncms??
				else if (!strcmp(me->name, "DEMAIN")) {
					if (!strcmp(me->value, "BLEU"))
						url += ETAT_JOUR::ETAT_JOUR_BLEU;
					else if (!strcmp(me->value, "BLAN"))
						url += ETAT_JOUR::ETAT_JOUR_BLANC;
					else if (!strcmp(me->value, "ROUG"))
						url += ETAT_JOUR::ETAT_JOUR_ROUGE;
					else
						url += ETAT_JOUR::ETAT_JOUR_INCONNU;
				}
				//fin ajout
				else {
					url += me->value;
				}
			} //not free entry
		} // While next
	} //if me
	// Json end
	url += "}";

	return url;
}



///* ======================================================================
//Function: build_emoncms_json string (usable by webserver.cpp)
//Purpose : construct the json part of emoncms url
//Input   : -
//Output  : String if some Teleinfo data available
//Comments: -
//====================================================================== */
//String webClient::build_emoncms_json(void)
//{
//
//  boolean first_item = true;
//  ESP.wdtFeed();  //Force software watchdog to restart from 0  ajout marc
//  String url = "{" ;
//
//  ValueList * me = TINFO.getList();
//  if (me) {
//      // Loop thru the node
//      while (me->next) {
//         if(! first_item) 
//          // go to next node
//          me = me->next;
//		 //marc
//		 else
//			 if (me->free) {
//				 //1st item is free : empty list !
//				 Debugln("Teleinfo list is empty !");
//				 break;
//			 }
//		 //fin marc
//         if( ! me->free ) {
//                
//             
//            // On first item, do not add , separator
//            if (first_item)
//              first_item = false;
//            else
//              url += ",";
//              
//              url +=  me->name ;
//              url += ":" ;
//      
//              // EMONCMS ne sait traiter que des valeurs numériques, donc ici il faut faire une 
//              // table de mappage, tout à fait arbitraire, mais c"est celle-ci dont je me sers 
//              // depuis mes débuts avec la téléinfo
//              if (!strcmp(me->name, "OPTARIF")) {
//                // L'option tarifaire choisie (Groupe "OPTARIF") est codée sur 4 caractères alphanumériques 
//                /* J'ai pris un nombre arbitraire codé dans l'ordre ci-dessous
//                je mets le 4eme char à 0, trop de possibilités
//                BASE => Option Base. 
//                HC.. => Option Heures Creuses. 
//                EJP. => Option EJP. 
//                BBRx => Option Tempo
//                */
//                char * p = me->value;
//                  
//                     if (*p=='B'&&*(p+1)=='A'&&*(p+2)=='S') url += "1";
//                else if (*p=='H'&&*(p+1)=='C'&&*(p+2)=='.') url += "2";
//                else if (*p=='E'&&*(p+1)=='J'&&*(p+2)=='P') url += "3";
//                else if (*p=='B'&&*(p+1)=='B'&&*(p+2)=='R') url += "4";
//                else url +="0";
//              } else if (!strcmp(me->name, "HHPHC")) {
//                // L'horaire heures pleines/heures creuses (Groupe "HHPHC") est codé par un caractère A à Y 
//                // J'ai choisi de prendre son code ASCII
//                int code = *me->value;
//                url += String(code);
//              } else if (!strcmp(me->name, "PTEC")) {
//                // La période tarifaire en cours (Groupe "PTEC"), est codée sur 4 caractères 
//                /* J'ai pris un nombre arbitraire codé dans l'ordre ci-dessous
//                TH.. => Toutes les Heures. 
//                HC.. => Heures Creuses. 
//                HP.. => Heures Pleines. 
//                HN.. => Heures Normales. 
//                PM.. => Heures de Pointe Mobile. 
//                HCJB => Heures Creuses Jours Bleus. 
//                HCJW => Heures Creuses Jours Blancs (White). 
//                HCJR => Heures Creuses Jours Rouges. 
//                HPJB => Heures Pleines Jours Bleus. 
//                HPJW => Heures Pleines Jours Blancs (White). 
//                HPJR => Heures Pleines Jours Rouges. 
//                */
//                     if (!strcmp(me->value, "TH..")) url += "1";
//                else if (!strcmp(me->value, "HC..")) url += "2";
//                else if (!strcmp(me->value, "HP..")) url += "3";
//                else if (!strcmp(me->value, "HN..")) url += "4";
//                else if (!strcmp(me->value, "PM..")) url += "5";
//                else if (!strcmp(me->value, "HCJB")) url += "6";
//                else if (!strcmp(me->value, "HCJW")) url += "7";
//                else if (!strcmp(me->value, "HCJR")) url += "8";
//                else if (!strcmp(me->value, "HPJB")) url += "9";
//                else if (!strcmp(me->value, "HPJW")) url += "10";
//                else if (!strcmp(me->value, "HPJR")) url += "11";
//                else url +="0";
//              } 
//			  //ajout pour affichage sur emoncms à préciser numérique sinon chaine
//			  //on pourrait supprimer demain, peu d'interet pour emoncms??
//			  else if (!strcmp(me->name, "DEMAIN")) {
//				  if (!strcmp(me->value, "BLEU"))
//					url += ETAT_JOUR::ETAT_JOUR_BLEU;
//				  else if (!strcmp(me->value, "BLAN"))
//					url += ETAT_JOUR::ETAT_JOUR_BLANC;
//				  else if (!strcmp(me->value, "ROUG"))
//					url += ETAT_JOUR::ETAT_JOUR_ROUGE;
//				  else
//					url += ETAT_JOUR::ETAT_JOUR_INCONNU;
//				  }
//			  //fin ajout
//			  else {
//                url += me->value;
//            }
//         } //not free entry
//      } // While next
//  } //if me
//  // Json end
//  url += "}";
//      
//  return url;
//}

/* ======================================================================
Function: emoncmsPost (called by main sketch on timer, if activated)
Purpose : Do a http post to emoncms
Input   : 
Output  : true if post returned 200 OK
Comments: -
====================================================================== */
boolean webClient::emoncmsPost(void)
{

	boolean ret = false;
  ST_message leMessage = ENREGISTREMENT.getEnregistrementTempsReel();
	DebugF("emoncmsPost");  Debugln(leMessage.dureeMax);
 /* if(leMessage.dureeMax)
  {*/
	  
	ESP.wdtFeed();
	  // Some basic checking
	  if (*CONFIGURATION.config.emoncms.host) {
		//ValueList * me = TINFO.getList();
		//// Got at least one ?
		//if (me && me->next) {
		  String url ; 
      

		  url = *CONFIGURATION.config.emoncms.url ? CONFIGURATION.config.emoncms.url : "/";
		  url += "?";
		  if (CONFIGURATION.config.emoncms.node>0) {
			url+= F("node=");
			url+= String(CONFIGURATION.config.emoncms.node);
			url+= "&";
		  } 

		  url += F("apikey=") ;
		  url += CONFIGURATION.config.emoncms.apikey;

		  //append json list of values
		  url += F("&json=") ;
		  url += build_emoncms_json_vmc(leMessage);  //Get Teleinfo list of values
		  // And submit all to emoncms
		  ret = httpPost(CONFIGURATION.config.emoncms.host, CONFIGURATION.config.emoncms.port, (char *) url.c_str()) ;
		//} // if me
	  } // if host
 //}
  return ret;
}

/* ======================================================================
Function: jeedomPost
Purpose : Do a http post to jeedom server
Input   : 
Output  : true if post returned 200 OK
Comments: -
====================================================================== */
boolean webClient::jeedomPost(void)
{
  ESP.wdtFeed();
  boolean ret = false;
  // Some basic checking
  if (*CONFIGURATION.config.jeedom.host) {
    ValueList * me = TINFO.getList();
    // Got at least one ?
    if (me && me->next) {
      String url ; 
      boolean skip_item;

      url = *CONFIGURATION.config.jeedom.url ? CONFIGURATION.config.jeedom.url : "/";
      url += "?";

      // Config identifiant forcée ?
      if (*CONFIGURATION.config.jeedom.adco) {
        url+= F("ADCO=");
        url+= CONFIGURATION.config.jeedom.adco;
        url+= "&";
      } 

      url += F("api=") ;
      url += CONFIGURATION.config.jeedom.apikey;
      url += F("&") ;

      // Loop thru the node
      while (me->next) {
        // go to next node
        me = me->next;
        skip_item = false;

        // Si ADCO déjà renseigné, on le remet pas
        if (!strcmp(me->name, "ADCO")) {
          if (*CONFIGURATION.config.jeedom.adco)
            skip_item = true;
        }

        // Si Item virtuel, on le met pas
        if (*me->name =='_')
          skip_item = true;

        // On doit ajouter l'item ?
        if (!skip_item) {
          url +=  me->name ;
          url += "=" ;
          url +=  me->value;
          url += "&" ;
        }
      } // While me

      ret = httpPost(CONFIGURATION.config.jeedom.host, CONFIGURATION.config.jeedom.port, (char *) url.c_str()) ;
    } // if me
  } // if host
  return ret;
}

/* ======================================================================
Function: HTTP Request
Purpose : Do a http request
Input   : 
Output  : true if post returned 200 OK
Comments: -
====================================================================== */
boolean webClient::httpRequest(void)
{
  boolean ret = false;

  // Some basic checking
  if (*CONFIGURATION.config.httpReq.host)
  {
    ValueList * me = TINFO.getList();
    // Got at least one ?
    if (me && me->next)
    {
      String url ; 
      boolean skip_item;

      url = *CONFIGURATION.config.httpReq.path ? CONFIGURATION.config.httpReq.path : "/";
      url += "?";

      // Loop thru the node
      while (me->next) {
        // go to next node
        me = me->next;
        skip_item = false;

        // Si Item virtuel, on le met pas
        if (*me->name =='_')
          skip_item = true;

        // On doit ajouter l'item ?
        if (!skip_item)
        {
          String valName = String(me->name);
			if (!this->modeLinkyHistorique) /* FOR STANDARD TYPE */
			{
				if (valName == "SINSTS") {
					url.replace("%SINSTS%", String(atol(me->value)));//Puissance app. Instantanée soutirée
				}
				if (valName == "EAST") {
					url.replace("%EAST%", String(atol(me->value)));	//Energie active soutirée totale
				}
				if (valName == "EASF01") {
					url.replace("%EASF01%", String(atol(me->value)));//Energie active soutirée Fournisseur, index 01
				}
				if (valName == "EASF02") {
					url.replace("%EASF02%", String(atol(me->value)));//Energie active soutirée Fournisseur, index 02
				}
			}
          if (valName == "HCHP")
          {
            url.replace("%HCHP%", me->value);
          }
          if (valName == "HCHC")
          {
            url.replace("%HCHC%", me->value);
          }
          if (valName == "PAPP")
          {
            url.replace("%PAPP%", me->value);
          }
		      if (valName == "ADCO")
          {
            url.replace("%ADCO%", me->value);
          }
		      if (valName == "OPTARIF")
          {
            url.replace("%OPTARIF%", me->value);
          }
		      if (valName == "ISOUSC")
          {
            url.replace("%ISOUSC%", me->value);
          }
		      if (valName == "PTEC")
          {
            url.replace("%PTEC%", me->value);
          }
		      if (valName == "IINST")
          {
            url.replace("%IINST%", me->value);
          }
		      if (valName == "IMAX")
          {
            url.replace("%IMAX%", me->value);
          }
		      if (valName == "HHPHC")
          {
            url.replace("%HHPHC%", me->value);
          }
		      if (valName == "MOTDETAT")
          {
            url.replace("%MOTDETAT%", me->value);
          }
		      if (valName == "BASE")
          {
            url.replace("%BASE%", me->value);
          }
          {
            url.replace("%BBRHCJB%", me->value);
          }
          {
            url.replace("%BBRHPJB%", me->value);
          }
          {
            url.replace("%BBRHCJW%", me->value);
          }
          {
            url.replace("%BBRHPJW%", me->value);
          }
          {
            url.replace("%BBRHCJR%", me->value);
          }
          {
            url.replace("%BBRHPJR%", me->value);
          }
	      }
      } // While me

      ret = httpPost(CONFIGURATION.config.httpReq.host, CONFIGURATION.config.httpReq.port, (char *) url.c_str()) ;
    } // if me
  } // if host
  return ret;
}

//********************************pour VMC**********************************


boolean webClient::emetEmoncmsJsonVmc(ST_message leMessage)
{
	String url=	this->build_emoncms_json_vmc(leMessage);
	return( httpPost(CONFIGURATION.config.emoncms.host, CONFIGURATION.config.emoncms.port, (char *)url.c_str()));
}

/* ======================================================================
Function: build_emoncms_json string (usable by webserver.cpp)
Purpose : construct the json part of emoncms url
Input   : -
Output  : String if some Teleinfo data available
Comments: -
====================================================================== */
String webClient::build_emoncms_json_vmc(ST_message leMessage)
{
	//{ADCO:039801172264, OPTARIF:4}
	//boolean first_item = true;
	//ESP.wdtFeed();  //Force software watchdog to restart from 0  ajout marc
	//String url = "{HEU:9999999999,CPT:9999999999,IIN:65535,DUR:65535,ETT:255,ETW:255,TPE:65535,TPC:65535,TPS:65535,HMC:255,HMS:255,ETV:255}";
//uint8_t posChamps[] = { 5,20,35,45,55,63,71,81,91,101,109,117 };

	//ST_message leMessage;
	/*leMessage.heure1970 = 3999999999;
	leMessage.compteur = 3999999999;
	leMessage.iInstMax = 255;
	leMessage.dureeMax = 65536;
	leMessage.etatTempo = 255;
	leMessage.etatWifi = 255;
	leMessage.tempExt = 32765;
	leMessage.tempCuis = -32765;
	leMessage.tempSdb = 32765;
	leMessage.humCuis = 255;
	leMessage.humSdb = 255;
	leMessage.etatVmc = 255;*/
	

	/*String nomChamps[] = { "timestamp:",",compteur:",",iInstMax:",",dureeMax:",",etatTempo:",",etatWifi:",",tempExt:",",tempCuis:",",tempSdb:",",humCuis:",",humSdb:",",mode:" ,",marche:" ,",vitesse:" };
	String url = "{";*/
	String nomChamps[] = { "DATE","compteur","IINST1","dureeMax","etatTempo","etatWifi","tempExt","tempCuis","tempSdb","humCuis","humSdb","mode" ,"marche" ,"vitesse" };
	String url = FPSTR(FP_JSON_START);
	url += "\"";
	url += nomChamps[0];
	url += FPSTR(FP_QCQ);
	url += String(leMessage.heure1970);
	url += FPSTR(FP_QCNL);
	url += nomChamps[1]; url += FPSTR(FP_QCQ); url += String(leMessage.compteur);   url += FPSTR(FP_QCNL);
	url += nomChamps[2]; url += FPSTR(FP_QCQ); url += String(leMessage.iInstMax);   url += FPSTR(FP_QCNL);
	url += nomChamps[3]; url += FPSTR(FP_QCQ); url += String(leMessage.dureeMax);   url += FPSTR(FP_QCNL);
	url += nomChamps[4]; url += FPSTR(FP_QCQ); url += String(leMessage.etatTempo);   url += FPSTR(FP_QCNL);
	url += nomChamps[5]; url += FPSTR(FP_QCQ); url += String(leMessage.etatWifi);   url += FPSTR(FP_QCNL);
	url += nomChamps[6]; url += FPSTR(FP_QCQ); url += String(leMessage.tempExt);   url += FPSTR(FP_QCNL);
	url += nomChamps[7]; url += FPSTR(FP_QCQ); url += String(leMessage.tempCuis);   url += FPSTR(FP_QCNL);
	url += nomChamps[8]; url += FPSTR(FP_QCQ); url += String(leMessage.tempSdb);   url += FPSTR(FP_QCNL);
	url += nomChamps[9]; url += FPSTR(FP_QCQ); url += String(leMessage.humCuis);   url += FPSTR(FP_QCNL);
	url += nomChamps[10]; url += FPSTR(FP_QCQ); url += String(leMessage.humSdb);   url += FPSTR(FP_QCNL);
	url += nomChamps[11]; url += FPSTR(FP_QCQ); url += String(leMessage.etatVmc>>2 & 3);   url += FPSTR(FP_QCNL);
	url += nomChamps[12]; url += FPSTR(FP_QCQ); url += String((leMessage.etatVmc>>1) & 1);   url += FPSTR(FP_QCNL);
	url += nomChamps[13]; url += FPSTR(FP_QCQ); url += String(leMessage.etatVmc & 1);
	url += F("\"");
	// Json end
	url += FPSTR(FP_JSON_END);
	return url;
}


