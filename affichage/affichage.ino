/*
 * affichage.ino
 *
 * Created: 24/02/2019 22:18:27
 *  Author: marc Prieur
 */
 //###################################include################################## 
//programmation: interface usb serial
//1 en bas (orange)
//terminal à 115200
//carte arduino pro ou pro mini
//choisir le bon fichier board.txt dans C:\Program Files (x86)\Arduino\hardware\arduino\avr(57200)
//charger affichage.ino
//arduino 1.8.7 ok
//arduino 1.8.19 ok
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
#ifdef HORLOGE
  #include <TM1637Display.h>
  #define CLK A5
  #define DIO A4
#endif
#include "dht.h" 
#include "st7735.h"
#include "canBus.h"  //add MCPCAN(modif chercher marc) to C:\Program Files (x86)\Arduino\hardware\arduino\avr\libraries for mcp_can.h to canBus.h
//WARNING MVPCAN modifié chercher marc
#include "ledsRgbSerial.h"
#include "ledsClassiques.h"
#include "poussoir.h"
#include "buzzer.h"
//###################################création des objets##################################  
//les mesures sont envoyées a l'uc de traitement et la moyenne est retournée pour affichage
dht DHTCUISINE( PIN_CAPTEUR_TEMP_HUMIDITE_CUISINE,0,0,10,1);   //la correction et l'ajustement décimal se font sur l'uc traitement
can_bus CAN_BUS(MCP_8MHZ);
st7735 AFFICHEUR;
ledsClassiques LEDS_CLASSIQUES;
ledsRgbSerial LEDS_RGB_SERIAL;
poussoir POUSSOIR;
buzzer BUZZER;
#ifdef  HORLOGE
    TM1637Display display = TM1637Display(CLK, DIO);
#endif
///###################################initialisations globales##################################  
int8_t task_1_sec = 0;
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
  Serial.println("setup");
#ifdef  HORLOGE
    display.setBrightness(4); //0 mini  7 maxi
    display.clear();
#endif
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
	struct_reception structReception;
	CAN_BUS.initialiseCanBus(); //a chaque cycle,si le distant est branché en second,inutile avec alimentation par le distant...
//#####################################Traitement des entrées######################################
 if(CAN_BUS.traitementReception()||(task_1_sec==3)) {   //sur la boucle sinon perte du 3° message   ATTENTION:pas de reception supplémentaire si la durée du cycle dépasse la seconde
	task_1_sec=0;
	structReception = CAN_BUS.getStructReception();
	retour = POUSSOIR.traitement(structReception.forcageMode);

#ifdef TRAITMODE
	//uint16_t decompteTempoArretMarcheForce=CAN_BUS.decDecompteTempoArretMarcheForce();
	//POUSSOIR.testModeForce(CAN_BUS.decDecompteTempoArretMarcheForce(), structReception.dureeForcage);
	POUSSOIR.testModeForce(structReception.decompteTempoArretMarcheForce);
#endif
	BUZZER.setBuzzer(retour);
	DHTCUISINE.DHT_T.clearMesure();		//n'efface pas mesure cycle,le cycle sans lecture reste sur la dernière valeur.
	DHTCUISINE.DHT_H.clearMesure();		// le clearMesure est nécessaire pour ne pas déborder sur les cumuls
	//au niveau affichage,le capteur est lu et envoyé sans correction à chaque cycle.
	retour = DHTCUISINE.lectureCapteur();
	BUZZER.setBuzzer(!retour);
//########################################Traitement Affichage##########################################
	LEDS_RGB_SERIAL.traitement(CAN_BUS.getStructReception());  //3 appels,compteur "en dur" dans getEtatReceptionInfos
	retour = LEDS_CLASSIQUES.traitement(CAN_BUS.getStructReception());
	BUZZER.setBuzzer(retour) ;
	AFFICHEUR.affiche(CAN_BUS.getStructReception());
	BUZZER.traitement(CAN_BUS.getStructReception());  //buzzer permanent si plus de réception CAN
//########################################Traitement des sorties##########################################
//les moyennes se1110.ront faite au niveaux des UC
	CAN_BUS.traitementEmission(DHTCUISINE.DHT_T.getMesureCycleMsb(),DHTCUISINE.DHT_T.getMesureCycleLsb(),DHTCUISINE.DHT_H.getMesureCycleMsb(),DHTCUISINE.DHT_H.getMesureCycleLsb(), POUSSOIR.getLeMode());
	CAN_BUS.clearStructReception();
//########################################Traitement temps des traitements##########################################
	//int32_t delta=1000-(memoTempsMilli-millis()) ;
	//Serial.print("temps libre:");Serial.println(delta);
 //################################################################################################################# 
#ifdef  HORLOGE
 // Print 1234 with the center colon:
 // display.showNumberDecEx(1234, 0b11100000, false, 4, 0);
  display.showNumberDecEx(structReception.heures, 0b01000000, false, 2, 0);
  display.showNumberDecEx(structReception.minutes, 0b01000000, false, 2, 2);  
#endif

   
  } //1 sec
//########################################Traitement durée du cycle##########################################
  if((millis()- memoTempsMilli) > 1000)
  {
    memoTempsMilli=millis();
    task_1_sec+=1;
  }
//##############################################################################################################
} 
//#############################################FIN####################################"
