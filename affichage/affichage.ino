/*
 * affichage.ino
 *
 * Created: 24/02/2019 22:18:27
 *  Author: marc Prieur
 */
 //###################################memoire################################## 
//04/01/2023
// au demarrage ,les 3 led restent sur nuit et rouge rouge a confirmer
//  et les 3 pavés sur l'ecran idem
//un changement d'état est testé au niveau des leds mais pas au niveau des paves
//est redevenu ok apres N minutes?
//les couleurs sont bonnes au niveau traitement
//l'heure etait bonne, meme message can bus que etatleds
//autre probleme peut-etre lie au probleme ci-dessus?
//en passant sur arret ou hiver, le decompte tourne sans arret.



//OK
//  Le croquis utilise 21844 octets (71%) de l'espace de stockage de programmes. Le maximum est de 30720 octets.
//  Les variables globales utilisent 1446 octets (70%) de mémoire dynamique, ce qui laisse 602 octets pour les variables locales. Le maximum est de 2048 octets.
//ajout HORLOGETM1637 OK
//  Le croquis utilise 22496 octets (73%) de l'espace de stockage de programmes. Le maximum est de 30720 octets.
//  Les variables globales utilisent 1469 octets (71%) de mémoire dynamique, ce qui laisse 579 octets pour les variables locales. Le maximum est de 2048 octets.
//ajout ALARME    probleme avec dht
//  Le croquis utilise 22718 octets (73%) de l'espace de stockage de programmes. Le maximum est de 30720 octets.
//  Les variables globales utilisent 1489 octets (72%) de mémoire dynamique, ce qui laisse 559 octets pour les variables locales. Le maximum est de 2048 octets.
//essai ALARME sans functions display   probleme avec dht
//  Le croquis utilise 22616 octets (73%) de l'espace de stockage de programmes. Le maximum est de 30720 octets.
//  Les variables globales utilisent 1479 octets (72%) de mémoire dynamique, ce qui laisse 569 octets pour les variables locales. Le maximum est de 2048 octets.

//ajout de F("xxx")
//  Le croquis utilise 22608 octets (73%) de l'espace de stockage de programmes. Le maximum est de 30720 octets.
//  Les variables globales utilisent 1167 octets (56%) de mémoire dynamique, ce qui laisse 881 octets pour les variables locales. Le maximum est de 2048 octets.

//21/06/23:
  //remplacement du bootloader pour watchdog
  //ajout relance watchdog 8s. dans loop

 //###################################include################################## 
//programmation: interface usb serial
//1 en bas (orange)
//terminal à 115200
//carte arduino pro ou pro mini
//choisir le bon fichier board.txt dans C:\Program Files (x86)\Arduino\hardware\arduino\avr(57600)
//ou modifier board.txt
//   carte Arduino Mini w/ ATmega328P
//mini.menu.cpu.atmega328.upload.speed=57600 
//ATTENTION fermer arduino et ne pas ouvrir a partir d'une fenetre arduino déja ouverte pour prise en compte modif
//charger affichage.ino
//arduino 1.8.7 ok
//arduino 1.8.19 ok
//***************************10/10/2022*****************************************
//modification commandes temporisees:voir traitement.ino
//******************************************************************************
#include <avr/wdt.h>
#include <gfxfont.h>  //add Adafruit-GFX-Library-master to arduino/libraries
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST77xx.h>  //add Adafruit-ST7735-Library-master to arduino/libraries
#include <Adafruit_ST7789.h>
#include <Adafruit_ST7735.h>
#include <NeoPixelBus.h>      //add NeoPixelBus-master to arduino/libraries
#include <NeoPixelBrightnessBus.h>
#include <NeoPixelAnimator.h>
#include <SPI.h>   //add SPI to C:\Program Files (x86)\Arduino\hardware\arduino\avr\libraries
#include <NewTone.h>  //add Newtone to arduino/libraries

#include "constantes.h"
#ifdef HORLOGETM1637
	#include <TM1637Display.h>
#endif
#ifdef HORLOGETM1650
	#include <TM1650.h>
#endif
#include "dht.h" 
#include "st7735.h"
#include "canBus.h"  //add MCPCAN(modif chercher marc) to C:\Program Files (x86)\Arduino\hardware\arduino\avr\libraries for mcp_can.h to canBus.h
//WARNING MVPCAN modifié chercher marc
#include "ledsRgbSerial.h"
//#include "ledsClassiques.h"
#include "poussoir.h"
#include "buzzer.h"
//###################################création des objets##################################  
//les mesures sont envoyées a l'uc de traitement et la moyenne est retournée pour affichage
dht DHTCUISINE( PIN_CAPTEUR_TEMP_HUMIDITE_CUISINE,0,0,10,1);   //la correction et l'ajustement décimal se font sur l'uc traitement
can_bus CAN_BUS(MCP_8MHZ);
st7735 AFFICHEUR;
//ledsClassiques LEDS_CLASSIQUES;
ledsRgbSerial LEDS_RGB_SERIAL;
poussoir POUSSOIR;
buzzer BUZZER;
int memoBrightness=2;
int memoMinute=-1;
#ifdef  HORLOGETM1650
//modifie C:\Users\mireille\Documents\Arduino\libraries\TM1650\src\TM1650.h TM1650_NUM_DIGITS   4 et TM1650_MAX_STRING   4 gain 
//reste 358 de mémoire dynamique avec cette modif 202 sans
	TM1650 display;
#endif
#ifdef  HORLOGETM1637
	int cptTest = 0;
	TM1637Display display(CLK, DIO);
#endif
///###################################initialisations globales##################################  
//int8_t task_1_sec = 0;
unsigned long memoTempsMilli=millis();  //cycle de 1 seconde sans RTC,on pourrait se caler sur la reception...
//######################################setup#####################################
//#ifdef  HORLOGE
  // Create array that turns all segments on:
//  const uint8_t data[] = {0xff, 0xff, 0xff, 0xff};
  // Create array that turns all segments off:
//  const uint8_t blank[] = {0x00, 0x00, 0x00, 0x00};
//#endif
void setup()
{
  Serial.begin(115200);
  Serial.println(F("setup"));

#ifdef  HORLOGETM1650 
  display.init();
	display.setBrightness(2); //0 mini  7 maxi  warning 0 is not mini il faut 1 à 8
#endif
#ifdef HORLOGETM1637
  display.setBrightness(2); //0 mini  7 maxi  warning 0 is not mini il faut 1 à 8
  display.showNumberDecEx(1234,0x40, true, 4, 0);  //40 pour :
#endif

    //display.setBrightness(memoBrightness); //0 mini  7 maxi
    //display.clear();
  AFFICHEUR.initAdafruit_ST7735();
  LEDS_RGB_SERIAL.init();
  BUZZER.test();
  delay(1000);
  BUZZER.test();
  delay(1000);
  BUZZER.test();
  memoTempsMilli = millis();
}
//########################################loop##########################################
//*********************************************BOUCLE 1 Seconde*******************************************
//*****************Mise en place du mode de fonctionnement à partir du bouton poussoir********************

void loop()
{
	boolean retour;
#ifdef  HORLOGETM1650
	char buf[4];
#endif
	struct_reception structReception;
	CAN_BUS.initialiseCanBus(); //a chaque cycle,si le distant est branché en second,inutile avec alimentation par le distant...
//#####################################Traitement des entrées######################################

//||(task_1_sec==3)  pour test probleme sur reinit canbus(supposition)
 if(CAN_BUS.traitementReceptionCan()) {   //sur la boucle sinon perte du 3° message   ATTENTION:pas de reception supplémentaire si la durée du cycle dépasse la seconde
	//task_1_sec=0;
	structReception = CAN_BUS.getStructReception();
//#ifdef TRAITMODE
//	retour = POUSSOIR.traitement(structReception.forcageMode);
//	//uint16_t decompteTempoArretMarcheForce=CAN_BUS.decDecompteTempoArretMarcheForce();
//	//POUSSOIR.testModeForce(CAN_BUS.decDecompteTempoArretMarcheForce(), structReception.dureeForcage);
//	//POUSSOIR.testModeForce(structReception.decompteTempoArretMarcheForce);
//#else
	retour = POUSSOIR.traitement(structReception.dureeForcageSec);
  if (!POUSSOIR.getTransitoirePoussoir())
  {
    //#endif
	  BUZZER.setBuzzer(retour);
	  DHTCUISINE.DHT_T.clearMesure();		//n'efface pas mesure cycle,le cycle sans lecture reste sur la dernière valeur.
	  DHTCUISINE.DHT_H.clearMesure();		// le clearMesure est nécessaire pour ne pas déborder sur les cumuls
	  //au niveau affichage,le capteur est lu et envoyé sans correction à chaque cycle.
	  retour = DHTCUISINE.lectureCapteur();
	  BUZZER.setBuzzer(!retour);
  //########################################Traitement Affichage##########################################
	  LEDS_RGB_SERIAL.traitement(CAN_BUS.getStructReception());  //3 appels,compteur "en dur" dans getEtatReceptionInfos
	  //retour = LEDS_CLASSIQUES.traitement(CAN_BUS.getStructReception());
	  BUZZER.setBuzzer(retour) ;
	  structReception.decompteTempoArretMarcheForce = POUSSOIR.getTempsMilliCommandeTemporisees();
    AFFICHEUR.affiche(structReception);
    wdt_enable(WDTO_8S);
    BUZZER.traitement(structReception);  //buzzer permanent si plus de réception CAN
    #ifdef  HORLOGETM1637
 // Print 1234 with the center colon:
 // display.showNumberDecEx(1234, 0b11100000, false, 4, 0);
  if (structReception.luminositeeLeds != memoBrightness)
  {
    memoBrightness = structReception.luminositeeLeds;
   if(memoBrightness>3)
    display.setBrightness(memoBrightness-2); //1 mini  8 maximemoBrightness+1
   else
    display.setBrightness(1); //1 mini  8 maxi
  }
  if (memoMinute!=structReception.minutes &&  structReception.infos)
  {
    memoMinute = structReception.minutes;
    display.showNumberDecEx(structReception.heures*100+structReception.minutes,0x40, true, 4, 0);  //40 pour :
    //display.showNumberDec(m, true, 2, 2);
    //display.showNumberDecEx(structReception.heures, 0b01000000, false, 2, 0);
    //display.showNumberDecEx(structReception.minutes, 0b01000000, false, 2, 2);
    }
#endif

#ifdef HORLOGETM1650
    // Print 1234 with the center colon:
    // display.showNumberDecEx(1234, 0b11100000, false, 4, 0);
    if (structReception.luminositeeLeds != memoBrightness)
    {
      memoBrightness = structReception.luminositeeLeds;
      display.setBrightness(memoBrightness); //0 mini  7 maxi
    }
    if (memoMinute!=structReception.minutes)
    {
      memoMinute = structReception.minutes;
      sprintf(buf, "%02d%02d", structReception.heures, structReception.minutes);  //with zero
      buf[1] = buf[1] | 0b10000000;   //second point
      display.displayString(buf);
      //display.showNumberDecEx(structReception.heures, 0b01000000, false, 2, 0);
      //display.showNumberDecEx(structReception.minutes, 0b01000000, false, 2, 2);
      }
#endif 
  } //getTransitoirePoussoir 
  else
  {
    AFFICHEUR.afficheMode(POUSSOIR.getLemodeTransitoire());
  }
  
	
//########################################Traitement des sorties##########################################
//les moyennes se1110.ront faite au niveaux des UC
//    Serial.print("Cuisine H  "); Serial.println(DHTCUISINE.DHT_H.getMesureCycleMsb());Serial.println(DHTCUISINE.DHT_H.getMesureCycleLsb());
//    Serial.print("Cuisine T  "); Serial.println(DHTCUISINE.DHT_T.getMesureCycleMsb());;Serial.println(DHTCUISINE.DHT_T.getMesureCycleLsb());
	CAN_BUS.traitementEmissionCan(DHTCUISINE.DHT_T.getMesureCycleMsb(),DHTCUISINE.DHT_T.getMesureCycleLsb(),DHTCUISINE.DHT_H.getMesureCycleMsb(),DHTCUISINE.DHT_H.getMesureCycleLsb(), POUSSOIR.getLeMode());
	CAN_BUS.clearStructReception();
//########################################Traitement temps des traitements##########################################
	//int32_t delta=1000-(memoTempsMilli-millis()) ;
	//Serial.print("temps libre:");Serial.println(delta);
 //################################################################################################################# 

  } //CAN_BUS.traitementReception()||(task_1_sec==3)
//########################################Traitement durée du cycle##########################################
  if((millis()- memoTempsMilli) > 1000)
  {
    memoTempsMilli=millis();
    //task_1_sec+=1;
  }
//#ifdef  HORLOGETM1637
//    // Print 1234 with the center colon:
//    display.showNumberDecEx(cptTest, 0x40, true, 4, 0);
//    cptTest += 1;
//    if (cptTest > 9999)
//      cptTest = 0;
//#endif
//##############################################################################################################
} 
//#############################################FIN####################################"
