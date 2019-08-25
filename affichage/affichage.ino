/*
 * affichage.ino
 *
 * Created: 24/02/2019 22:18:27
 *  Author: marc Prieur
 */
 //###################################include##################################  
#include <gfxfont.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST77xx.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_ST7735.h>
#include <NeoPixelBus.h>
#include <NeoPixelBrightnessBus.h>
#include <NeoPixelAnimator.h>
#include <SPI.h>
#include <mcp_can_dfs.h>
#include <mcp_can.h>
#include <NewTone.h>
#include "constantes.h"
#include "dht.h"
#include "st7735.h"
#include "canBus.h"
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
///###################################initialisations globales##################################  
int8_t task_1_sec = 0;
unsigned long memoTempsMilli=millis()+1000;  //cycle de 1 seconde sans RTC,on pourrait se caler sur la reception...
//######################################setup#####################################
void setup()
{
  Serial.begin(115200);
  Serial.println("setup");
  AFFICHEUR.initAdafruit_ST7735();
  LEDS_RGB_SERIAL.init();
  BUZZER.test();
  delay(1000);
  BUZZER.test();
  delay(1000);
  BUZZER.test();
}
//########################################loop##########################################
//*********************************************BOUCLE 1 Seconde*******************************************
//*****************Mise en place du mode de fonctionnement à partir du bouton poussoir********************
void loop()
{
	boolean retour;
	CAN_BUS.initialiseCanBus(); //a chaque cycle,si le distant est branché en second,inutile avec alimentation par le distant...
//#####################################Traitement des entrées######################################
 if(CAN_BUS.traitementReception()||(task_1_sec==3)) {   //sur la boucle sinon perte du 3° message   ATTENTION:pas de reception supplémentaire si la durée du cycle dépasse la seconde
	task_1_sec=0;
	retour = POUSSOIR.traitement();
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
//les moyennes seront faite au niveaux des UC
	CAN_BUS.traitementEmission(DHTCUISINE.DHT_T.getMesureCycleMsb(),DHTCUISINE.DHT_T.getMesureCycleLsb(),DHTCUISINE.DHT_H.getMesureCycleMsb(),DHTCUISINE.DHT_H.getMesureCycleLsb(), POUSSOIR.getLeMode());
	CAN_BUS.clearStructReception();
//########################################Traitement temps des traitements##########################################
	//int32_t delta=1000-(memoTempsMilli-millis()) ;
	//Serial.print("temps libre:");Serial.println(delta);
 //#################################################################################################################   
  } //1 sec
//########################################Traitement durée du cycle##########################################
  if(millis()>= memoTempsMilli)
  {
    memoTempsMilli=millis()+1000;
    task_1_sec+=1;
  }
//##############################################################################################################
} 
//#############################################FIN####################################"
