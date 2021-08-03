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
char TRAME[][NBCHAR] = { "\x02\0",
"\rBBRHCJB 000000000 \0", "\rBBRHCJW 000000000 \0", "\rBBRHCJR 000000000 \0",
"\rBBRHPJB 000000000 \0", "\rBBRHPJW 000000000 \0", "\rBBRHPJR 000000000 \0",
"\rADCO 039801172264 \0", "\rOPTARIF BBR* \0", "\rISOUSC 45 \0",
"\rPTEC HCJB \0",	"\rDEMAIN BLEU \0",	"\rIINST 001 \0","\rIMAX 025 \0",
"\rHHPHC Y \0","\rMOTDETAT 000000 \0","\x03\0" };
 char Etiq[][13] = { "STX", "BBRHCJB","BBRHCJW","BBRHCJR","BBRHPJB","BBRHPJW","BBRHPJR", "ADCO", "OPTARIF", "ISOUSC", "PTEC", "DEMAIN", "IINST", "IMAX", "HHPHC", "MOTDETAT" };
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
unsigned char calcChecksum(char *mot)
{
	unsigned int  sum = ' ';  // Somme des codes ASCII du message + un espace
							  // avoid dead loop, always check all is fine 
	if (mot) {
		// this will not hurt and may save our life ;-)
		if (strlen(mot)) {
			while (*mot)
				sum += *mot++;
			return ((sum & 63) + ' ');
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
void SimuTempo::traite1champ(char * trame, char * format, int longueur, int longueurEntete, int indice, unsigned long variable)
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
void ICACHE_FLASH_ATTR SimuTempo::initSimuTrameTempo(void)
{
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
	traite1champ(&TRAME[ADCO][0], format12, 12, 6, ADCO, secondes);
	for (int cpt = CPTJBN; cpt<CPTJRJ + 1; cpt++)
	{
		traite1champ(&TRAME[cpt][0], format9, 9, 9,  cpt, compteur[cpt]);
	}
	traite1champ(&TRAME[IINST][0], format3, 3, 7, IINST, randMinMax(0L, 255L));
	traite1champ(&TRAME[PTEC][0],  4, 6, PTEC, &tableauPtecSimu[compteurCourant][0]);
	traite1champ(&TRAME[DEMAIN][0], 4, 8, DEMAIN, &tableauDemain[couleurDemain][0]);
	//emission d'une trame compl�te
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
}



