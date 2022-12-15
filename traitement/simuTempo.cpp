// **********************************************************************************
// Programme traitement ESP8266 Pilotage VMC et TEMPO->Simulation de messages tempo version historique
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
#include <Arduino.h>
#include "Wifinfo.h"
#include "mySyslog.h"
#include "LibTeleinfo.h"
#include "constantes.h"
#include "simuTempo.h"
#define NO_DEBUGSIMU
//OK FIXE:HHPHC,IINST,MOTDETAT,IMAX
//OK EVOL:PTEC,DEMAIN,
//1 seul compteur 1 fois BBRHPJR � 0
//pb OPTARIF:0000013081SOUSC
//manque 5 compteurs,ADCO,ISOUSC dans OPTARIF
//plantage au load esp8266 si tableaux dans la classe
//char TRAME[][NBCHAR] = { "\x02\0","\rADCO 039801172264 \0", "\rOPTARIF BBR* \0", "\rISOUSC 45 \0",
//"\rBBRHCJB 000000000 \0", "\rBBRHPJB 000000000 \0", "\rBBRHCJW 000000000 \0",
//"\rBBRHPJW000000000 \0", "\rBBRHCJR 000000000 \0", "\rBBRHPJR 000000000 \0",
//"\rPTEC HCJB \0",	"\rDEMAIN BLEU \0",	"\rIINST 001 \0","\rIMAX 025 \0",
//"\rHHPHC Y \0","\rMOTDETAT 000000 \0","\x03\0" };
#ifdef MODE_HISTORIQUE
  char TRAME[][NBCHAR] = { "\x02\0",
  "\nBBRHCJB 000000000 \0", "\nBBRHCJW 000000000 \0", "\nBBRHCJR 000000000 \0",
  "\nBBRHPJB 000000000 \0", "\nBBRHPJW 000000000 \0", "\nBBRHPJR 000000000 \0",
  "\nADCO 039801172264 \0", "\nOPTARIF BBR* \0", "\nISOUSC 45 \0",
  "\nPTEC HCJB \0",	"\nDEMAIN BLEU \0",	"\nIINST 001 \0","\nIMAX 025 \0",
  "\nHHPHC Y \0","\nMOTDETAT 000000 \0","\x03\0" };
   char Etiq[][13] = { "STX", "BBRHCJB","BBRHCJW","BBRHCJR","BBRHPJB","BBRHPJW","BBRHPJR", "ADCO", "OPTARIF",
   "ISOUSC", "PTEC", "DEMAIN", "IINST", "IMAX", "HHPHC", "MOTDETAT" };
  // _separator = 0x20;
#else
	//_separator =2*TINFO_HT;
//champs evolutifs
	//EASF01(9) a EASF06   6 compteurs remplace BBRHxx  9 char
	//NJOURF(2) remplace PTEC
	//NJOURF+1(2) remplace DEMAIN	
	//IRMS1(3) remplace IINST
	//RELAIS(3) remplace MOTDETAT a voir
//champs constants
	//LTARF(16) tarif remplace OPTARIF
	//ADSC(12) remplace ADCO
	//ajout:
	//SMAXSN(5) horodatee

	//enleve:
	//"\rISOUSC 45 \0",
	// "\rIRMS1 001 \0"
	//"\rIMAX 025 \0",
	//"\rHHPHC Y \0",E200927185432
  char TRAME[][NBCHAR] = { "\x02\0",
  "\nEASF01\t000000001\t\0", "\nEASF02\t000000002\t\0", "\nEASF03\t000000003\t\0",
  "\nEASF04\t000000004\0", "\nEASF05\t000000005\t\0", "\nEASF06\t000000006\t\0",
  "\nADSC\t000123456789\t\0", "\nLTARTF\tBBR)\t\0", 
  "\nNJOURF\t01\t\0","\nNJOURF+1\t02\t\0",
  "\nIRMS1\t010\t\0",  "\nRELAIS\t011\t\0","\nSMAXSN\t12345\tE200927185432\t\0","\x03\0" };  //,"\nDATE\tE200927185432\t\0"
  char Etiq[][13] = {  "ADSC", "VTIC", "DATE", "NGTF", "LTARF", "EAST","EASF01","EASF02","EASF03","EASF04",  //10
  "EASF05","EASF06", "EASF07", "EASF08", "EASF09", "EASF10", "EASD01", "EASD02", "EASD03", "EASD04",  //10
  "EAIT", "ERQ1", "ERQ2", "ERQ3", "ERQ4", "IRMS1", "IRMS2", "IRMS3", "URMS1", "URMS2",  //10
  "URMS3","PREF","PCOUP", "SINSTS","SINSTS1","SINSTS2","SINSTS3","SMAXSN","SMAXSN1","SMAXSN2",  //10
  "SMAXSN3","SMAXSN-1","SMAXSN1-1","SMAXSN2-1","SMAXSN3-1","SINSTI","SMAXIN","SMAXIN-1","CCASN","CCASN-1",  //10
  "CCAIN","CCAIN-1","UMOY1","UMOY2","UMOY3","STGE","DPM1","FPM1","DPM2","FPM2", //10
  "DPM3","FPM3","MSG1","MSG2","PRM","RELAIS","NTARF","NJOURF","NJOURF+1","PPOINTE"};
#endif
 char tableauPtecSimu[][5] = { "STX", "HCJB", "HCJW", "HCJR", "HPJB", "HPJW", "HPJR" };
 char tableauDemain[][5] = { "BLEU", "BLAN", "ROUG" };
unsigned int compteur[7] = { 0,0,0,0,0,0,0 };
#define NO_X86   //pour x86


//options suppl�mentaire du compilateur, ajouter -fpermissive 
//ATTENTION a tabnames de webserver.cpp si version LibTeleinfo-syslog.zip
//avec esp12 0.9
//strapper ces 2 pins:
//emission sur serial1 esp12-->GPIO2--->TXD1-->D4
//r�ception sur serial apr�s le swap-->GPIO13---->RXD2--->D7
//fonctions calcChecksum de LibTeleinfo modifi�
unsigned char SimuTempo::calcChecksum(char *mot)
{
//mode standard revoir si utilisation horodate
	unsigned int  sum = _separator;  // Somme des codes ASCII du message + un espace
							  // avoid dead loop, always check all is fine 
	if (mot) {
		// this will not hurt and may save our life ;-)
		if (strlen(mot)) {
			while (*mot)
				sum += *mot++;
			return ((sum & 0x3f) + 0x20);
		}
	}
	return 0;
}
/* ======================================================================
Function: initSimuTrame
Purpose :
Input   :
Output  : -
Comments: -
====================================================================== */
int SimuTempo::randMinMax(int min, int max)
{
	return rand() % (max - min + 1) + min;
}
void SimuTempo::traite1champ(char * trame, char * format, int longueur, int longueurEntete, int indice, unsigned long variable,bool horodate)
{
	char buffer[13];
#ifdef X86
	sprintf_s(buffer, format, variable);
#else
	sprintf(buffer, format, variable);
#endif
	for (int i = 0; i < longueur; i++)
		trame[i + longueurEntete] = buffer[i];
	trame[longueur + longueurEntete + 1] = TINFO.calcChecksum(&Etiq[indice][0], &buffer[0]);
}
void SimuTempo::traite1champ(char * trame, int longueur, int longueurEntete, int indice, char * buffer)
{
	/*char buffer[13];
	sprintf(buffer, format, variable);*/

	for (int i = 0; i < longueur; i++)
		trame[i + longueurEntete] = buffer[i];
	trame[longueur + longueurEntete + 1] = TINFO.calcChecksum(&Etiq[indice][0], &buffer[0]);
}
//version historique
//NBCHAMP(17)nombre d'information
//pour chaque info(group):TINFO_SGR ,infos de N char (max 22)END_STR , checksum,TINFO_EGR,END_STR
//data max 22:etiquette 4 à 8 ,1 blanc,donnee 1 a 12 ,1 blanc
void ICACHE_FLASH_ATTR SimuTempo::initSimuTrameTempo(void)
{
#ifdef MODE_HISTORIQUE
     _separator = 0x20;
#else
  _separator =2*TINFO_HT;
#endif
	for (int i = 1; i < NBCHAMP - 1; i++)
	{
		TRAME[i][0] = TINFO_SGR;
		for (int j = 0; j <NBCHAR; j++)
		{
			if (TRAME[i][j] == END_STR)
			{
				TRAME[i][j] = calcChecksum(&TRAME[i][1]);			//CRC			
				TRAME[i][j + 1] = TINFO_EGR;
				TRAME[i][j + 2] = END_STR;
				break;
			}
		}
	}
}
bool passeEmetTrame = false;
void SimuTempo::emetTrameTempo( unsigned long secondes,int compteurCourant, unsigned int  couleurDemain)
{
	char format12[] = { "%012lu" };
	char format9[] = { "%09lu" };
	char format3[] = { "%03lu" };
	char format2[] = { "%02lu" };
	//modification des champs evolutifs
#ifdef MODE_HISTORIQUE
//CPT1 a 6	1 a 6
//ADCO		7
//PTEC		10
//DEMAIN	11
//IINST		12
	for (int cpt = CPTJBN; cpt<CPTJRJ + 1; cpt++)
	{
		traite1champ(&TRAME[cpt][0], format9, 9, 9,  cpt, compteur[cpt], false);
	}
	traite1champ(&TRAME[ADCO][0], format12, 12, 6, ADCO, secondes, false);
	traite1champ(&TRAME[PTEC][0],  4, 6, PTEC, &tableauPtecSimu[compteurCourant][0]);
	traite1champ(&TRAME[DEMAIN][0], 4, 8, DEMAIN, &tableauDemain[couleurDemain][0]);
	traite1champ(&TRAME[IINST][0], format3, 3, 7, IINST, randMinMax(0L, 255L), false);
#else
	for (int cpt = EASF01; cpt< EASF06 + 1; cpt++)
	{
		traite1champ(&TRAME[cpt][0], format9, 9, 9,  cpt, compteur[cpt],false);
	}
	//traite1champ(&TRAME[ADSC][0], format12, 12, 6, ADSC, secondes, false);
	traite1champ(&TRAME[NJOURF][0], format2, 2, 8, NJOURF, (unsigned long)compteurCourant, false);
	traite1champ(&TRAME[NJOURFPLUS1][0], format2 ,2, 10, NJOURFPLUS1, couleurDemain, false); //attention etiquette=NJOURF+1=>longueurEntete=8+2
	traite1champ(&TRAME[IRMS1][0], format3, 3, 7, IRMS1, (unsigned long)randMinMax(0L, 255L), false);
	// Les bits n°4 à 10 indiquent l’état des 7 contacts virtuels.
	traite1champ(&TRAME[RELAY][0], format3, 3, 8, RELAY, 140UL, false);   //exemple doc bits 3,4,8  

#endif
	//emission d'une trame complete
#ifdef X86
		printf("secondes %d\r\n", secondes);
#endif

	for (int i = 0; i < NBCHAMP; i++)
	{
		for (int j = 0; j < NBCHAR; j++)
		{
			if (TRAME[i][j] != END_STR)
			{
#ifdef  X86
				if (TRAME[i][j] > 0x20 & TRAME[i][j] < 0x60)
					printf("%c.", TRAME[i][j]);
				else
					printf("%x-", TRAME[i][j]);
#else
				char caract[3];
				SerialSimu.write(TRAME[i][j]);
				if (!passeEmetTrame)
				{
					if ((TRAME[i][j] > 0x20) & (TRAME[i][j] < 0x60))
					{
						sprintf(caract, "%c", TRAME[i][j]);
						Debug(caract);
					}
					else
					{
						sprintf(caract, "%x", TRAME[i][j]);
						DebugF("-");Debug(caract);DebugF("-");
					 }
				}
#endif
			}
			else
			{
#ifdef X86
				printf("\r\n");
#else
				if (!passeEmetTrame)
					DebuglnF("");
#endif
				break;
			}
		}
#ifndef X86
		//delay(10);//tempo inter champ
#endif
	}
	//cpt += 1;  pas de test de d�bordement...
#ifndef X86
	delay(10);//tempo intertrame
#endif
	passeEmetTrame = true;
}

void SimuTempo::traite1Trame( unsigned long secondes)
{
#ifdef MODE_HISTORIQUE
	static	int compteurCourant = CPTJBN;
	static	unsigned int  couleurDemain = ETAT_JOUR_BLEU;
	if (!(secondes % 5))  //�volution ptec et demain
	{
		compteurCourant += 1;
		if (compteurCourant > CPTJRJ)
			compteurCourant = CPTJBN;
		couleurDemain += 1;
		if (couleurDemain > ETAT_JOUR_ROUGE)
			couleurDemain = ETAT_JOUR_BLEU;
	}
	if(!(secondes % 2)) //�volution compteur courant et �mission
	{
		compteur[compteurCourant] += randMinMax(100L, 999L);
		if (compteur[compteurCourant] > (999999999L - 999L))
			compteur[compteurCourant] = 0;
		emetTrameTempo(secondes, compteurCourant, couleurDemain);
	}

#else
	//Le format utilisé pour les horodates est SAAMMJJhhmmss, c'est-à-dire Saison, Année, Mois, Jour, heure, minute, seconde.
	//S=H hiver
	//S=E ete
	//S en minuscule si probleme de synchro
	//S=espace dans certain cas
	static	int compteurCourant = EASF01;
	static	unsigned int  couleurDemain = ETAT_JOUR_BLEU;
	if (!(secondes % 5))  //�volution ptec et demain
	{
		compteurCourant += 1;
		if (compteurCourant > EASF06)
			compteurCourant = EASF01;
		couleurDemain += 1;
		if (couleurDemain > ETAT_JOUR_ROUGE)
			couleurDemain = ETAT_JOUR_BLEU;
	}
	if(!(secondes % 2)) //�volution compteur courant et �mission
	{
		compteur[compteurCourant] += randMinMax(100L, 999L);
		if (compteur[compteurCourant] > (999999999L - 999L))
			compteur[compteurCourant] = 0;
		emetTrameTempo(secondes, compteurCourant, couleurDemain);
	}
#endif
}
