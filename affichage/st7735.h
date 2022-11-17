// **********************************************************************************
// ESP8266 Pilotage VMC et TEMPO->Gestion de l'affichage sur un ST7735.
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
#ifndef ST7735_H_
#define ST7735_H_
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
//*************definition des lignes*****************
#define HEIGHT 160
#define HAUTLIGNE  18
#define V_NBLIGNE 9
#define V_TXTLIGNETITRE  2
#define V_TXTLIGNEMODES  V_TXTLIGNETITRE
#define V_TXTLIGNETCUIS  (V_TXTLIGNETITRE+HAUTLIGNE)
#define V_TXTLIGNETSDB  (V_TXTLIGNETCUIS+HAUTLIGNE)
#define V_TXTLIGNETEXT  (V_TXTLIGNETSDB+HAUTLIGNE)
#define V_TXTLIGNEHCUIS  (V_TXTLIGNETEXT+HAUTLIGNE)
#define V_TXTLIGNEHSDB  (V_TXTLIGNEHCUIS+HAUTLIGNE)
#ifdef ALARME
  #define V_TXTLIGNEALARME  (V_TXTLIGNEHSDB+HAUTLIGNE)
  #define V_TXTLIGNEHEURE  (V_TXTLIGNEALARME+HAUTLIGNE)
#else
  #define V_TXTLIGNEHEURE  (V_TXTLIGNEHSDB+HAUTLIGNE)
#endif
#define V_TXTLIGNEETAT  (V_TXTLIGNEHEURE+HAUTLIGNE)
#define V_TXTSTATUS     V_TXTLIGNEHCUIS
#define V_TXTSTATUS1    V_TXTLIGNEHSDB
//*************definition des colonnes*****************
#define WIDTH 128
#define V_COLCONSTANTES  2
#define V_COLMODE  2
#define V_COLVARIABLES  63
#define V_COLHEURE  2
#define V_COLTEMPSRESTANT
#define V_COLERREUR    109
#define COL1  2
#define COL2  15
#define COLHEURE  35
#define COL3  49
#define COL5  116
#ifdef ALARME
  #define XLABELALARMEGARAGE V_COLCONSTANTES
  #define XPAVEALARMEGARAGE V_COLCONSTANTES + 40  
  #define XLABELALARMEPORTAIL V_COLVARIABLES  
  #define XPAVEALARMEPORTAIL V_COLVARIABLES + 40
#endif
#define XPAVEJOUR 3
#define XPAVEDEMAIN 18
#define XPAVEJOURNUIT 33
#define XPAVELABELWIFI 48
#define XPAVEWIFI XPAVELABELWIFI+4
#define XPAVETEMPSRESTANT XPAVELABELWIFI+22+5

//**************definition des couleurs**********************
//https://ee-programming-notepad.blogspot.com/2016/10/16-bit-color-generator-picker.html
#define COLORSCREEN ST7735_BLACK              //0xAFB7                    //bleu pale ST7735_GREEN
#define COLORCONSTANTBACK ST7735_BLACK
#define COLORCONSTANTFORE ST7735_CYAN    //ST7735_YELLOW
#define COLORCADRES ST7735_ORANGE
#define COLORBLUEDAYS ST7735_BLUE
#define COLORWHITEDAYS ST7735_WHITE
#define COLORREDDAYS ST7735_RED
#define COLORREDDAYSWIFIOK ST7735_GREEN
#define COLORREDDAYSWIFIPB ST7735_ORANGE
#define COLORNIGHT ST7735_BLACK
#define COLORDAY ST7735_BLUE
//#define COLORVARIABLESBACK ST7735_ORANGE
#define COLORVARIABLESFORE ST7735_WHITE     //ST7735_RED
#define COLORVARIABLESMODES ST7735_WHITE
//********************************************************
//#define ST7735_BLACK      ST77XX_BLACK
//#define ST7735_WHITE      ST77XX_WHITE
//#define ST7735_RED        ST77XX_RED
//#define ST7735_GREEN      ST77XX_GREEN
//#define ST7735_BLUE       ST77XX_BLUE
//#define ST7735_CYAN       ST77XX_CYAN
//#define ST7735_MAGENTA    ST77XX_MAGENTA
//#define ST7735_YELLOW     ST77XX_YELLOW
//#define ST7735_ORANGE     ST77XX_ORANGE
//#define DELTALINESTEXT    3
//#define NBLIGNE 7
//#define NBPIXPARLETTERSIZE2    14
class st7735 : public Adafruit_ST7735
{
  public:
    st7735();
    void affiche(struct_reception R);  //uint8_t heure,uint8_t minute,uint8_t secondes,float  temperature,float humidite
    void initAdafruit_ST7735(void);
    void setchangeMode(bool changeMode);
    void afficheMode(MODES modeSelection );
    void setMode( int mode);
  private:
    bool changeMode = false;
    int modeSelection = 0;
    void afficheDistants(struct_reception R);
    void afficheLocaux(struct_reception R);
    void fix_number_position(float number);
    void fix_number_position(int number);
    void TraitePave(int X,uint8_t etat);
    void TraitePaveJN(int X,uint8_t etat);
#ifdef ALARME
        void TraitePaveAlarme(int X, uint8_t etat);
#endif
    void TraiteLigneEtat(struct_reception R);
    void initScreen(void);
    void afficheFloat(float temperature, unsigned char positionX, unsigned char positionY);
    void afficheInt(int humidity, unsigned char positionX, unsigned char positionY);
    void debugInfos(struct_reception R);
    void debugDistants(struct_reception R);
    void debugLocaux(struct_reception R);
    void changementMode(void);
    void decompteCgtVitesse(struct_reception R);
    void casNormal(struct_reception R);
    bool changeModePrec = false;
    uint8_t clignote = 12;
    uint8_t tourniquet = 4;
};
extern st7735 AFFICHEUR;
#endif /* ST7735_H_ */
