// ***************************************************************************************
// ESP8266 Pilotage VMC et TEMPO->Constantes commune � affichage et � traitement VMC/TEMPO
// ***************************************************************************************
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
//#########################ATTENTION:fichier commun affichage et traitement_tempo#######################################################
//              il faut le charger dans les 2 projets pour arduino qui charge tous les fichiers situes dans le dossier du .ino

#ifndef CONSTANTESPARTAGEES_h
#define CONSTANTESPARTAGEES_h

enum MODES { ARRET = 0, PETITE_VITESSE, GRANDE_VITESSE, AUTO, TEMPO_MARCHE_PV, TEMPO_MARCHE_GV, TEMPO_ARRET,ETE,HIVER,BIDON};
enum LES_ID_CAN { ID_MESSAGE_TYPE_0 = 1, ID_MESSAGE_TYPE_1, ID_MESSAGE_TYPE_2, ID_MESSAGE_TYPE_3, ID_MESSAGE_TYPE_4, ID_MESSAGE_TYPE_5};  //11 bits maxi  2047 
enum MESSAGE_TYPE_0 { HEURE = 0, MINUTE, SECONDE, LES_LEDS, MODE, ETAT, LEBUZZER, COURANTVMC, FIN_MESSAGE_TYPE_0 };										//infos uc vers affichage
enum MESSAGE_TYPE_1 { DHT_SDB_T_MSB = 0, DHT_SDB_T_LSB, DHT_SDB_H_MSB, DHT_SDB_H_LSB, TEMP_EXT_MSB, TEMP_EXT_LSB, SEUILAUTO, CASAUTO, FIN_MESSAGE_TYPE_1 };     //capteurs  uc vers affichage
//#ifdef TRAITMODE
//	enum MESSAGE_TYPE_2 { DHT_CUISINE_T_MSB = 0, DHT_CUISINE_T_LSB, DHT_CUISINE_H_MSB, DHT_CUISINE_H_LSB, DECOMPTE_FORCAGE , FIN_MESSAGE_TYPE_2 };					//capteurs affichage vers uc
//#else
	enum MESSAGE_TYPE_2 { DHT_CUISINE_T_MSB = 0, DHT_CUISINE_T_LSB, DHT_CUISINE_H_MSB, DHT_CUISINE_H_LSB, FIN_MESSAGE_TYPE_2 };					//capteurs affichage vers uc
//#endif

enum MESSAGE_TYPE_3 { NOUV_MODE, FIN_MESSAGE_TYPE_3 };	//infos affichage vers uc
#ifdef ALARME
	enum MESSAGE_TYPE_4 { AFF_CUISINE_T_MSB = 0, AFF_CUISINE_T_LSB, AFF_CUISINE_H_MSB, AFF_CUISINE_H_LSB,ETAT_WIFI,LUMINOSITE_LEDS_RGB, ALARME_1, ALARME_2, FIN_MESSAGE_TYPE_4 };					//capteurs  uc vers affichage
#else
	enum MESSAGE_TYPE_4 { AFF_CUISINE_T_MSB = 0, AFF_CUISINE_T_LSB, AFF_CUISINE_H_MSB, AFF_CUISINE_H_LSB,ETAT_WIFI,LUMINOSITE_LEDS_RGB, FIN_MESSAGE_TYPE_4 };					//capteurs  uc vers affichage
#endif
//#ifdef TRAITMODE
enum MESSAGE_TYPE_5 { DECOMPTE_TEMPO_ARRET_MARCHE_FORCE_MSB, DECOMPTE_TEMPO_ARRET_MARCHE_FORCE_MIDDLE, DECOMPTE_TEMPO_ARRET_MARCHE_FORCE_LSB, CPT_DELAI_CGT_VITESSE, NB_MINUTE_JOUR_COURANT_MSB, NB_MINUTE_JOUR_COURANT_LSB, MARCHE_ARRET, FORCAGE_MODE, FIN_MESSAGE_TYPE_5 };   //traitement uc vers affichage
//#else
//enum MESSAGE_TYPE_5 { DECOMPTE_TEMPO_ARRET_MARCHE_FORCE_MSB, DECOMPTE_TEMPO_ARRET_MARCHE_FORCE_MIDDLE, DECOMPTE_TEMPO_ARRET_MARCHE_FORCE_LSB, CPT_DELAI_CGT_VITESSE, NB_MINUTE_JOUR_COURANT_MSB, NB_MINUTE_JOUR_COURANT_LSB, MARCHE_ARRET, FIN_MESSAGE_TYPE_5 };   //traitement uc vers affichage
//#endif
enum ERREURS { E_TEMPEXT = 1, E_DHTCUISINE = 2, E_DHTSDB = 4, E_TA12 = 8, E_CAN_BUS_TRAIT = 0x10, E_CAN_BUS_AFF = 0x20, E_TINFO = 0x30 };
enum CASSTATUS { ST_START = 0, ST_TROPCHAUD, ST_TROPFROID, ST_CUISTROPHUM, ST_SDBTROPHUM,ST_ROUGEJOUR, ST_FIN };

#define VITESSE_CAN CAN_250KBPS

#endif
