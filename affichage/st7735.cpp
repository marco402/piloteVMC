// Programme affichage  arduino pro mini  Pilotage VMC et TEMPO->Gestion de l'affichage sur un ST7735.
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
//Using library Adafruit-GFX-Library-master version 1.3.6
//Using library Adafruit-ST7735-Library-master version 1.2.7
//Using library SPI version 1.0 in folder
// **********************************************************************************
/* 09/2022:ajout alarme  reception.alarmeGarage

*/
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <stdio.h>
#include "constantes.h"
#include "canBus.h"
#include "dht.h"
#include "st7735.h"
//d�tails TFT + lien pour les couleurs en RGB565
//http://henrysbench.capnfatz.com/henrys-bench/arduino-adafruit-gfx-library-user-guide/arduino-adafruit-gfx-printing-to-the-tft-screen/
//shield achet�:https://www.arthurwiz.com/software-development/177-inch-tft-lcd-display-with-st7735s-on-arduino-mega-2560
//1  GND
//2 VCC                     5V      3.1 � 6V r�gulateur 662k sur la carte------>3.3V
//3 SCK   SCLK  clock           D13   adaptation 5v->3.3v   
//4 SDA   SDA   serial data MOSI      D11   adaptation 5v->3.3v   
//5 RES   RESX  reset           3.3V  direct
//6 RS    D/CX  register selection      D4    adaptation 5v->3.3v   
//7 CS    CSX   chip select         D8    adaptation 5v->3.3v   
//8 LEDA  luminosit�e 3.3V ou pot. de 10k       via 1k
extern "C" void __cxa_pure_virtual() { while (1); }
//#ifdef ALARME
//  enum enCouleursALARME{ST7735_WHITE = 0, ST7735_RED, ST7735_GREEN,ST7735_BLUE,ST7735_ORANGE};
//#endif
uint16_t couleursWIFI[] = {ST7735_RED, ST7735_RED, ST7735_ORANGE, ST7735_GREEN};
uint16_t couleursTEMPO[] = {ST7735_BLUE, ST7735_WHITE, ST7735_RED, COLORSCREEN};
#ifdef ALARME
  //uint16_t couleursALARME[] = {enCouleursALARME::ST7735_BLACK, enCouleursALARME::ST7735_RED, enCouleursALARME::ST7735_GREEN,enCouleursALARME::ST7735_BLUE,enCouleursALARME::ST7735_ORANGE};
  uint16_t couleursALARME[] = {ST7735_BLACK,ST7735_RED,ST7735_GREEN,ST7735_BLUE,ST7735_ORANGE};
#endif
uint16_t couleursJOURNUIT[] = { COLORSCREEN, ST7735_ORANGE };
// print pb avec string
char MODES_AFF[][11] = {"" , "  ARRET   ","   LENT   "," RAPIDE   ","  AUTO    ","FORCE PV  ","FORCE GV  ","FORC ARRET","    ETE   ","   HIVER  ","AT CAN BUS","cas inex" }; //blanc n�cessaires pour effacer la plus longue chaine
//String MODES_AFF[] = { "  ARRET   ","   LENT   "," RAPIDE   ","  AUTO    ","FORCE PV  ","FORCE GV  ","FORC ARRET","AT CAN BUS" }; //blanc n�cessaires pour effacer la plus longue chaine
char CAS_AUTO[][12] = { "           ","A-temp ext>","A-temp ext<","M-hum cuis>","M-hum  sdb>","cas inex   "};  //      ,"   arret   " ,"SHC","SHB","SCHAUD","SFROID"};
#define NO_DEBUG_ST7735

st7735::st7735() : Adafruit_ST7735(TFT_CS, TFT_RS, TFT_RESET)           //-------->cycle d'affichage autour de 800ms.
{
}
void st7735::initAdafruit_ST7735(void)
{
  initScreen(); //pas initScreen dans le constructeur
  pinMode(TFT_LEDA, OUTPUT);
  //1023--->tout a 1
//900
//800
//700
//600
//500
//400
//300
//200
//100
//0------>tout a 0
  analogWrite(TFT_LEDA, 1023); 
}
void st7735::initScreen(void) 
{
  initR(INITR_BLACKTAB);  //INITR_GREENTAB inverse les couleurs(BRG) et d�cale d'une ligne
  //commentaires lettre jaune sur fond bleu
  setTextColor(COLORCONSTANTFORE, COLORCONSTANTBACK);
  fillScreen(COLORSCREEN);
#ifdef HORIZONTAL
  setRotation(1);     //1:horizontal connecteur a droite
  setTextSize(2);
  setCursor(COL3 + DELTALINESTEXT, TXTLIGNE2);
  print(F("TEMP"));
  setCursor(COL5 + DELTALINESTEXT, TXTLIGNE2);
  print(F("HUM"));
  setCursor(COL1, TXTLIGNE3);
  print(F("Cuis"));
  setCursor(COL1, TXTLIGNE4);
  print(F("SdB"));
  setCursor(COL1, TXTLIGNE5);
  print(F("Ext"));
  //setTextColor(ST7735_CYAN, ST7735_BLUE);  
  setCursor(COL1, TXTLIGNE6);
  print(F("I Vmc(A)"));
  //setCursor(COLHEURE, TXTLIGNE7);
  //print("h");
  //setCursor(COLHEURE + NBPIXPARLETTERSIZE2 * 3, TXTLIGNE7);   //75 
  //print("mn");
  //setCursor(COLHEURE + NBPIXPARLETTERSIZE2 * 7, TXTLIGNE7);  //124
  //print("s");

  //cadres vert   
  drawRect(0, 0, 160, 128, COLORCADRES);
  //lignes horizontales
  for (int oneLine = 0; oneLine < NBLIGNE; oneLine++)
    drawLine(0, HAUTLIGNE*oneLine, 160, HAUTLIGNE*oneLine, COLORCADRES);
  //lignes verticales
  drawLine(COL3, HAUTLIGNE, COL3, HAUTLIGNE * 4, COLORCADRES);
  drawLine(COL5 + DELTALINESTEXT, HAUTLIGNE, COL5 + DELTALINESTEXT, HAUTLIGNE * 5, COLORCADRES);

  //fillRect(3, HAUTLIGNE + 2, 12, HAUTLIGNE - 3, COLORBLUEDAYS);
  //fillRect(18, HAUTLIGNE + 2, 12, HAUTLIGNE - 3, COLORWHITEDAYS);
  //fillRect(33, HAUTLIGNE + 2, 12, HAUTLIGNE - 3, COLORREDDAYS);

  //pour les variables
  setTextColor(COLORVARIABLESFORE, COLORREDDAYSWIFIOK);
#else
  setRotation(0);     //0:vertical  connecteur en bas
  //commentaires fixes
  setTextSize(2);
  setCursor(V_COLCONSTANTES, V_TXTLIGNETCUIS);
  print(F("T cui"));
  setCursor(V_COLCONSTANTES, V_TXTLIGNETSDB);
  print(F("T sdb"));
  setCursor(V_COLCONSTANTES, V_TXTLIGNETEXT);
  print(F("T ext"));
  setCursor(V_COLCONSTANTES, V_TXTLIGNEHCUIS);
  print(F("H cui"));
  setCursor(V_COLCONSTANTES, V_TXTLIGNEHSDB);
  print(F("H sdb"));
#ifdef ALARME
  setCursor(XLABELALARMEGARAGE, V_TXTLIGNEALARME);
  print(F("Gar"));  //Al Garage
  setCursor(XLABELALARMEPORTAIL, V_TXTLIGNEALARME);
  print(F("Por"));  //Al Portail
#endif
  //cadres vert
  drawRect(0, 0, WIDTH, HEIGHT, COLORCADRES);
  //lignes horizontales
  for (int oneLine = 0; oneLine < V_NBLIGNE; oneLine++)
    drawLine(0, HAUTLIGNE*oneLine, WIDTH, HAUTLIGNE*oneLine, COLORCADRES);
  //lignes verticales
  drawLine(V_COLVARIABLES-1, HAUTLIGNE, V_COLVARIABLES - 1, HAUTLIGNE * 8, COLORCADRES);
  //pour les variables
  setTextColor(COLORVARIABLESFORE, COLORSCREEN);
#endif
}

void st7735::changementMode(void)
{
  if (!changeModePrec)
  {
    fillScreen(COLORSCREEN);
    changeModePrec = true;
  }
  setCursor(V_COLMODE + 3, V_TXTLIGNEMODES);
  setTextColor(ST7735_ORANGE, COLORSCREEN);
  print(MODES_AFF[modeSelection]);
}
void st7735::afficheMode(MODES modeSelection )
{
  //fillScreen(COLORSCREEN);
  setCursor(V_COLMODE + 3, V_TXTLIGNEMODES);
  setTextColor(ST7735_ORANGE, COLORSCREEN);
  print(MODES_AFF[modeSelection]);
}
void st7735::decompteCgtVitesse(struct_reception R)
{
  fillScreen(COLORSCREEN);
  setCursor(V_COLMODE, V_TXTSTATUS);
  print(F("Decompte "));
  setCursor(V_COLMODE, V_TXTSTATUS1);
  print(F("relais: "));
  print(R.decompteDelaiCgtVitesse);
  changeModePrec = true;
}
void st7735::casNormal(struct_reception R)
{
  if (changeModePrec)
  {
    changeModePrec = false;
    initScreen();
  }
  if (R.infos)
  {
    //int8_t puis = 0;
    //*******************traitement du mode et arret marche***************************************
    setCursor(V_COLMODE + 3, V_TXTLIGNEMODES);
    if (R.arret_marche == ARRET_MARCHE::MARCHE_REL)
    {
      setTextColor(ST7735_BLACK, ST7735_GREEN);
      //puis = R.puissanceVMC;
    }
    else
      setTextColor(ST7735_WHITE, COLORSCREEN);
    if (R.NbMessage)
    {
      uint8_t mode = R.mode;
      if (mode > 9)
        mode = 10;    //cas inex
      print(MODES_AFF[mode]);
    }
    else
      print(F("Pas de mes."));
    setTextColor(COLORVARIABLESFORE, COLORSCREEN);
    //********************traitement puissance vmc**************************************
    //afficheInt((float)puis, V_COLVARIABLES + 12, V_TXTLIGNEIVMC);
    //********************traitement alarme garage**************************************
//V_COLVARIABLES + 12  V_TXTLIGNEIVMC
#ifdef ALARME
    TraitePaveAlarme(XPAVEALARMEGARAGE, R.alarmeGarage);
    TraitePaveAlarme(XPAVEALARMEPORTAIL, R.alarmePortail); 
//          Serial.print(F("alarmeGarage: "));Serial.println(R.alarmeGarage);  //54 violet
//          Serial.print(F("alarmePortail: "));Serial.println(R.alarmePortail); //4  orange         
#endif
    //TraitePaveAlarme(V_COLVARIABLES + 30, R.alarmePortail);
    //******************traitement des pav�s tempo et wifi*******************************
    union leds etatDesLeds;
    etatDesLeds.etat = R.etatLeds;
    TraitePave(XPAVEJOUR, etatDesLeds.etatCourant.aujourdhui);
    TraitePave(XPAVEDEMAIN, etatDesLeds.etatCourant.demain);
    TraitePaveJN(XPAVEJOURNUIT, etatDesLeds.etatCourant.jourNuit);    //1=nuit sinon 0
    //Serial.print("wifi:"); Serial.println(reception.etatWifi);
    TraiteLigneEtat(R);
    //*******************traitement heure*****************************
    //char HeureCourante[9];   //reception.NbMinuteActiveJourCourant
    //sprintf(HeureCourante, "%02u:%02u:%02u", R.heures, R.minutes, R.secondes);  //ATTENTION aux sprintf, cause des pb leds
    fillRect(V_COLHEURE, V_TXTLIGNEHEURE, WIDTH-V_COLHEURE-1, HAUTLIGNE - 2, COLORSCREEN);
    setTextColor(COLORVARIABLESFORE, COLORSCREEN);
    setCursor(V_COLHEURE, V_TXTLIGNEHEURE);
    print(R.heures);
    print(F(":"));
    print(R.minutes);
    print(F(":"));
    print(R.secondes);
    //**************************affichage erreur******************************
    if (R.Rbuzzer != 0)       // on reste sur la derni�re erreur
    {
      setCursor(V_COLERREUR, V_TXTLIGNEHEURE + 5);
      setTextSize(1);
      print(F("e:"));
      print(R.Rbuzzer);
      setTextSize(2);
    }
  }
  else               //*******pas de reception can***************
  {
    setCursor(V_COLMODE + 3, V_TXTLIGNEMODES);
    if (clignote<6)
      setTextColor(ST7735_ORANGE, COLORREDDAYSWIFIPB);
    else
      setTextColor(ST7735_ORANGE, COLORSCREEN);
    print(MODES_AFF[9]);
    clignote -= 1;
    if (!clignote)
      clignote = 12;
  }
  setTextColor(COLORVARIABLESFORE, COLORSCREEN);
  //**********************affichage des capteurs*********************
  afficheDistants(R);
  afficheLocaux(R);
}
void st7735::affiche(struct_reception reception)    //uint8_t heure,uint8_t minute,uint8_t secondes,float  temperature,float humidite
{
  //pour utiliser le meme parametre ecran et led rgb--> pas cablé pour le st7735
 analogWrite(TFT_LEDA,(512 * reception.luminositeeLeds)/7);  //0 to 1023
  //return;
//132*162

//caract�res 8*5 size 1(d�faut)   27 chars 14 lignes
//      16*10 size 2      13      7
//      24*15 size 3      6     3

//    heure minute seconde            08h 24mn 55s
//    buzzer
//    mode en cours               MODE FORCE GV
//    etat
//    etatleds
//    courant vmc                 I VMC:     4.2A
  //********************traitement du changement de mode via poussoir**************************
  if (changeMode)
    changementMode();
  //*************************traitement du d�compte relais*********************************
  else if (reception.decompteDelaiCgtVitesse > 0)
    decompteCgtVitesse(reception);
  //************************traitement cas courant**************************************
  else
    casNormal(reception);
}

#ifdef ALARME
void st7735::TraitePaveAlarme(int X, uint8_t etat)
{
  fillRect(X, V_TXTLIGNEALARME + 2, 12, HAUTLIGNE - 8, couleursALARME[etat]);
}
#endif
void st7735::TraitePave(int X, uint8_t etat)
{
  fillRect(X, V_TXTLIGNEETAT + 2, 12, HAUTLIGNE - 8, couleursTEMPO[etat ]);
}
void st7735::TraitePaveJN(int X, uint8_t etat)
{
  fillRect(X, V_TXTLIGNEETAT + 2, 12, HAUTLIGNE - 8, couleursJOURNUIT[etat ]);
}
void st7735::TraiteLigneEtat(struct_reception reception)
{
  fillRect(XPAVELABELWIFI, V_TXTLIGNEETAT, 79, HAUTLIGNE - 5, COLORSCREEN);
  setTextSize(1);
  setCursor(XPAVELABELWIFI, V_TXTLIGNEETAT + 5);
  tourniquet -= 1;
  if (!tourniquet)
    tourniquet = 4;
  switch (tourniquet)
  {
  case 1:
    if ((reception.etatWifi & 3) < 3)   //pas de wifi
    {
      print(F("Wi"));
      fillRect(XPAVEWIFI + 10, V_TXTLIGNEETAT + 2, 14, HAUTLIGNE - 8, couleursWIFI[reception.etatWifi & 3]);
    }
    break;
  case 2:
    //**********************traitement seuil et cas auto********************
    if (reception.decompteTempoArretMarcheForce > 0)
    {
      print(F("dec:"));
      print((reception.decompteTempoArretMarcheForce-(unsigned long) millis())/1000);
    }
    break;
  case 3:
    if (reception.casAuto == CASSTATUS::ST_START || reception.mode != 3)
    {
      //**********************traitement temps restant********************
      print(F("tvmc:"));
      print(reception.NbMinuteActiveJourCourant);
    }
    break;
  case 4://etat mode auto
      CASSTATUS cas =(CASSTATUS) reception.casAuto;
      if (cas > CASSTATUS::ST_FIN)
        cas = CASSTATUS::ST_FIN;
      print(CAS_AUTO[cas]);
      print(reception.seuilAuto);
    break;
  }
  setTextSize(2);
}
void st7735::afficheDistants(struct_reception reception)    //uint8_t heure,uint8_t minute,uint8_t secondes,float  temperature,float humidite
{
//    temp�rature salle de bain         TEMP SDB:  18�
//    humidit� salle de bain            HUM SDB:   48%
//    temp�rature ext�rieur
  if(reception.distants)
  {
  afficheFloat((float)reception.temperature_sdb_aff/10.0f, V_COLVARIABLES , V_TXTLIGNETSDB);
  afficheInt(reception.humidite_sdb_aff, V_COLVARIABLES+12, V_TXTLIGNEHSDB);
  afficheFloat((float)reception.temperature_ext_aff/10.0f, V_COLVARIABLES, V_TXTLIGNETEXT);
  #ifdef DEBUG_ST7735
  debugDistants(reception);
  #endif
  }
}
void st7735::afficheLocaux(struct_reception reception)    //uint8_t heure,uint8_t minute,uint8_t secondes,float  temperature,float humidite
{
//    temp�rature cuisine             TEMP CUIS: 22� 
//    humidit� cuisine              HUM CUIS:  45%
  if(reception.locaux)
  {
    afficheFloat((float)reception.temperature_cuis_aff / 10.0f, V_COLVARIABLES, V_TXTLIGNETCUIS);
    afficheInt(reception.humidite_cuis_aff, V_COLVARIABLES+12, V_TXTLIGNEHCUIS);
#ifdef DEBUG_ST7735
  debugLocaux(reception);
#endif
  }
  else
  {
    //en local,ne tient pas compte des corrections �ventuelles
    afficheFloat((float)DHTCUISINE.DHT_T.getMesureCycle() / 10.0f, V_COLVARIABLES, V_TXTLIGNETCUIS);
    afficheInt(DHTCUISINE.DHT_H.getMesureCycle(), V_COLVARIABLES+12, V_TXTLIGNEHCUIS);
  }
}
void st7735::afficheFloat(float param, unsigned char positionX, unsigned char positionY)
{
  setCursor(positionX, positionY);
  fix_number_position(param);
  print(param, 1);
}
void st7735::afficheInt(int param, unsigned char positionX, unsigned char positionY)
{

  setCursor(positionX, positionY);
  print(F("    "));                      //pour nettoyer la zone
  setCursor(positionX, positionY);
  fix_number_position(param);
  print(param);
}
void st7735::debugInfos(struct_reception R)
{
  Serial.print(R.heures);Serial.print(F("-"));Serial.print(R.minutes);Serial.print(F("-"));Serial.print(R.secondes );Serial.print(F("-"));
  Serial.print(R.Rbuzzer);Serial.print(F("-"));Serial.print(R.mode);Serial.print(F("-"));Serial.print(R.etatWifi);Serial.print(F("-"));Serial.print(R.etatLeds);
}
void st7735::debugDistants(struct_reception R)
{
  Serial.print(R.temperature_sdb_aff / 10.0f);
  Serial.print(F("-"));
  Serial.print(R.humidite_sdb_aff);
  Serial.print(F("-"));
  Serial.println(R.temperature_ext_aff / 10.0f);
}
void st7735::debugLocaux(struct_reception R)
{
  Serial.print(R.temperature_cuis_aff / 10.0f);
  Serial.print(F("-"));
  Serial.println(R.humidite_cuis_aff);
}
void st7735::setchangeMode(bool changeMode)
{
  this->changeMode = changeMode;
}
void st7735::setMode(int modeSelection)
{
  this->modeSelection = modeSelection;
}
void st7735::fix_number_position(float number)
{
  if ((number >= -9.9) && (number < 0.0))    //-9.9      -0.1        4 char--->1 blanc
    print(F(" "));
  else if ((number >= 0.0) && (number < 10.0))    //0.0       9.9         3 char--->2 blanc
    print(F("  "));
  else if ((number >= 10.0) && (number < 100.0))   //10.0       99.9        4 char--->1 blanc
    print(F(" "));
}
void st7735::fix_number_position(int number)
{
  if (number < 10)
    print(F(" "));
}
