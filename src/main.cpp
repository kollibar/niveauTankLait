#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
//#include <Adafruit_SH1106.h>
#include <IO_SH1106_xD.h>
#include <HC_SR04.h>
#include "brochage.h"
#include <OneWire.h>
#include <Clavier_xD.h>
#include "calibrage.h"
#include <OneWire.h>
#include <EEPROM.h>

#define DEBUG


#define OLED_RESET 4
IO_SH1106_xD display(OLED_RESET,touches,3);
//Adafruit_SH1106 display(OLED_RESET);
//Clavier_xD clavier=Clavier_xD(touches, 3);

HC_SR04 capteurDist=HC_SR04(broche_trigger,broche_echo);


OneWire ds=OneWire(broche_onewire);

unsigned long dateMesureTemp; // date de la dernière demande de mesure

bool chercheSonde(byte addr[]){

  ds.reset_search(); // reset le bus 1wire

  if (!ds.search(addr)) {
    // Pas de capteur
    Serial.print('p');
    return false;
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
    // Adresse invalide
    Serial.print('c');
    return false;
  }

  /* Vérifie qu'il s'agit bien d'un DS18B20 */
  if (addr[0] != 0x28) {
    // Mauvais type de capteur
    Serial.print('n');
    return false;
  }

  return true;
}

bool demandeMesureTemp(const byte addr[]) {
  /* demande le lancement d'une mesure de température sur le bus 1wire, à la sonde d'addr donnée. La mesure sera dispo dans 750ms */

  if( (millis() - dateMesureTemp) < 1000 ) return true; // il y a déjà une demande de mesure de température effetuée il y a moins de 800 ms

  ds.reset();             // On reset le bus 1-Wire
  ds.select(addr);        // On sélectionne le DS18B20
  ds.write(0x44, 1);      // On lance une prise de mesure de température
  dateMesureTemp=millis();
  return true;
}

signed int litTemperature(const byte addr[]) {
  /* lit la temperature de la dernière mesure effctué avec la sonde à l'addr donnée*/

  unsigned char data[9];

  ds.reset();             // On reset le bus 1-Wire
  ds.select(addr);;        // On sélectionne la sonde
  ds.write(0xBE,1);         // On envoie une demande de lecture du scratchpad

  for (unsigned char i = 0; i < 9; ++i) // On lit le scratchpad
    data[i] = ds.read();       // Et on stock les octets reçus

  // Calcul de la température en 1/16e de degré Celsius


  return ( ((data[1] << 8) | data[0]) );
}


bool mesureTempFaite(){
  return ((millis() - dateMesureTemp) > 800 && (millis() - dateMesureTemp) < 5000);
}

signed int waitForMesureTemp(const byte addr[]) {

  unsigned long delai = millis() - dateMesureTemp;
  if( delai > 800 && delai < 5000 ) return litTemperature(addr); // mesure déjà faite

  if( delai > 800)  {
    demandeMesureTemp(addr); // si pas de mesure déjà faite on en demande une
    delai=800;
  } else {
    delai=800-delai;
  }

  if( delai >= 0){
    delay(delai);
  }

  signed int mesure =litTemperature(addr);

  if( mesure == 1360 ){ // erreur de mesure, on réessaye une 2e fois (seulement une 2e fois)
    demandeMesureTemp(addr);
    delai=800;
    delay(delai);
    mesure=litTemperature(addr);
  }

  return mesure;
}

signed int getTemperature(const byte addr[]) {
  /* demande une mesure de la température et attend (800ms) que la mesure soit faite, puis retourne la valeur */


  unsigned long delai=800;

  if( millis() - dateMesureTemp < delai){ // une demande de mesure de température a été effctuée il y a moins de 800 ms
    return waitForMesureTemp(addr);
  }


  demandeMesureTemp(addr);
  delay(delai); // si le noyau FreeRTOS est inactif
  signed long mesure=litTemperature(addr);

  if( mesure == 1360 ){ // erreur de mesure, on réessaye une 2e fois (seulement une 2e fois)
    demandeMesureTemp(addr);
    delay(delai); // si le noyau FreeRTOS est inactif
    mesure=litTemperature(addr);
  }

  return mesure;
}


signed int calcVolumeTank(unsigned int mesureTmps){
  #ifdef DEBUG
  Serial.print(F("calcVolumeTank("));
  Serial.print(mesureTmps);
  Serial.println(F(")      #######"));
  #endif

  signed long mesure = (signed long)mesureTmps;

  if( mesure > 4320 ) return 0;

  #ifdef DEBUG
    Serial.print(F("mesure:"));
    Serial.print(mesure);
  #endif

  //signed long posPrec=mesure/30;
  //signed long posSuiv=mesure/30+1;

  //signed long precedent=pgm_read_word_near(CALIBRAGE+mesure/30);
  //signed long suivant=pgm_read_word_near(CALIBRAGE+mesure/30+1);

  signed long distPrec=abs(mesure-30*(mesure/30));
  signed long distSuiv=abs(mesure-30*(mesure/30)-30);

  #ifdef DEBUG
  Serial.println(F("preced"));
  Serial.print(F(" pos:"));
  Serial.println(mesure/30);
  Serial.print(F(" dist:"));
  Serial.println(distPrec);
  Serial.println();

  Serial.println(F("suivant"));
  Serial.print(F(" pos:"));
  Serial.println(mesure/30+1);
  Serial.print(F(" dist:"));
  Serial.println(distSuiv);
  Serial.println();

  Serial.print(F(" resultat:"));


  Serial.print((signed int)
  ((pgm_read_word_near(CALIBRAGE+mesure/30)*abs(mesure-30*(mesure/30))+pgm_read_word_near(CALIBRAGE+mesure/30+1)*abs(mesure-30*(mesure/30+1))))/(distPrec+distSuiv));

  Serial.println();
  #endif



  return (signed int)((pgm_read_word_near(CALIBRAGE+mesure/30)*distPrec+pgm_read_word_near(CALIBRAGE+mesure/30+1)*distSuiv)/(distPrec+distSuiv));
}

void setup()   {

  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  capteurDist.begin();

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  interrupteur.begin();
  //clavier.begin();
  // init done

  display.clearDisplay();
  display.display();

  display.setTextSize(1);
  display.setTextColor(WHITE);

  pinMode(broche_chauffe, OUTPUT);
  digitalWrite(broche_chauffe, LOW);

  display.print(F("start"));
  display.display();
  #ifdef DEBUG
  Serial.println(F("start"));
  #endif
}

/***** POUR LE CALIBRAGE *****
void loop() {
  unsigned char touche=0;
  unsigned long date=0;
  display.aucuneTouche(true);

  display.clearDisplay();
  display.setTextSize(2);
  display.print("Calibrage");
  display.setTextSize(1);
  display.setCursor(0,24);
  display.print("attente démarrage");
  display.display();

  while( touche != BT_SELECT) {
    touche=display.getToucheAct(true);
    if( millis() - date  > 500){
      display.setCursor(0,48);
      display.fillRect(0, 48, 124, 24, BLACK);

      unsigned int mesure=capteurDist.mesureTmps();
      display.print(mesure);
      display.print("µs ");
      display.print((((long)mesure)*340)/2000);
      display.print("mm");
      display.display();
      date=millis();
      Serial.println(mesure);
      if( touche == BT_SELECT) Serial.println("BT_SELECT");
    }
  }
  touche=0;

  display.aucuneTouche(true);
  display.setTextSize(3);
  // start
  unsigned long time=0;

  while(touche != BT_SELECT){

    touche=display.getToucheAct(true);
    if( millis() - date  > 500){
      date=millis();
      display.setCursor(0,24);
      display.fillRect(0,24,128,48,BLACK);
      display.print(capteurDist.mesureMoyenne(5));
      display.display();

      if( millis() - time > 5000 ){
        time=millis();
        Serial.print(time);
        Serial.print(':');
        Serial.println(capteurDist.mesureTmpsMoyenne(5));
      }
    }
  }
  touche=0;
  display.aucuneTouche(true);
  display.clearDisplay();
  display.print("fin");
  display.display();
  display.getTouche(-2);
}
***********************/

const char AUCUNE_ALERTE  = 0;
const char ALERTE_BAS     = 1;
const char ALERTE_MOYEN   = 2;
const char ALERTE_HAUT    = 3;

const signed int NIV_ALERTE_BAS   = 8000;
const signed int NIV_ALERTE_MOYEN = 8200;
const signed int NIV_ALERTE_HAUT  = 8400;

const unsigned long ALERTE_BAS_PERIODE_ON    = 80;
const unsigned long ALERTE_MOYEN_PERIODE_ON  = 80;
const unsigned long ALERTE_HAUT_PERIODE_ON   = 70;

const unsigned long ALERTE_BAS_PERIODE_OFF    = 1200;
const unsigned long ALERTE_MOYEN_PERIODE_OFF  = 800;
const unsigned long ALERTE_HAUT_PERIODE_OFF   = 70;

const unsigned int ALERTE_BAS_FREQUENCE   = 440;
const unsigned int ALERTE_MOYEN_FREQUENCE = 880;
const unsigned int ALERTE_HAUT_FREQUENCE  = 880;


void loop(){
  unsigned char addr[8];

  if( !chercheSonde(addr) ) {
    addr[0]=0;
  }
  signed int tempConsigne;
  // chargement de la température de consigne en EEPROM
  tempConsigne=((signed int)(EEPROM.read(0)))<<8 | ((signed int)(EEPROM.read(1)));
  if( tempConsigne > 1440 || tempConsigne < 160 ) tempConsigne=960;

  unsigned long date=0;
  unsigned long dateTemp=4200000000;
  unsigned long dateNiveau=4200000000;
  unsigned long dateAlarme=0;
  unsigned long dateEnregistrement=0;
  unsigned char alerte=AUCUNE_ALERTE;
  unsigned char touche,lastTouche=0;
  bool alarmeConnue=false;
  /*unsigned long perdiodeAlarmeOn=0;
  unsigned long perdiodeAlarmeOff=0;
  unsigned int frequenceAlarme=0;*/
  bool etatAlarme=false;
  display.clearDisplay();
  for(;;){
    #ifdef DEBUG
    Serial.println(F("Boucle"));
    #endif
    date=millis();

    if( dateEnregistrement != 0 && date >= dateEnregistrement){
      // enregistrement de la température de consigne en EEPROM
      EEPROM.write(1,(signed char)(tempConsigne&0xFF));
      EEPROM.write(0,(signed char)tempConsigne>>8);
    }

    if( alerte != ALERTE_HAUT && alerte != AUCUNE_ALERTE && ! alarmeConnue && touches[2].active()) {
      alarmeConnue=true;
      noTone(broche_buzzer);
    }

    lastTouche=touche;
    touche=display.getTouche(0);
    if( (touche & TIMEOUT) != TIMEOUT && touche != lastTouche) {
      if( interrupteur.active() ){ // température de consigne active
        if( touche == BT_HAUT && tempConsigne < 1440 ) {
          tempConsigne+=0x10;
          dateEnregistrement=date+10000;
        }
        else if( touche == BT_BAS && tempConsigne > 160 ) {
          tempConsigne -= 0x10;
          dateEnregistrement=date+10000;
        }
      }
    }

    if( date - dateNiveau > 1000 ){ // actualisatino affichage niveau toutes les secondes
      dateNiveau=date;
      display.fillRect(0, 0, 128, 32, BLACK);
      display.setCursor(0,0);
      display.setTextSize(3);
      unsigned int nivMesureTmps=capteurDist.mesureTmpsMoyenne(2);
      signed int nivVolume=calcVolumeTank(nivMesureTmps);

      #ifdef DEBUG
      Serial.print("mesure temps:");
      Serial.print(nivMesureTmps);
      Serial.print("    distance:");
      Serial.print(capteurDist.calcDist(nivMesureTmps));
      Serial.print("    volume:");
      Serial.println(nivVolume/10);
      #endif

      display.print(nivVolume/10);
      display.print('L');

      if( nivVolume > NIV_ALERTE_HAUT){
        alerte=ALERTE_HAUT;
        alarmeConnue=false;
      } else if( nivVolume > NIV_ALERTE_MOYEN ){
        if( alerte != ALERTE_MOYEN ) {
          alarmeConnue=false;
          alerte=ALERTE_MOYEN;
        }
      } else if( nivVolume > NIV_ALERTE_BAS ){
        if( alerte < ALERTE_BAS ) alarmeConnue = false;
        alerte=ALERTE_BAS;
      } else if( alerte != AUCUNE_ALERTE){ // pas d'alerte
        alerte = AUCUNE_ALERTE;
        alarmeConnue=false;
        noTone(broche_buzzer);
      }

      display.setTextSize(1);
      if( alerte == AUCUNE_ALERTE){
        display.print(capteurDist.calcDist(nivMesureTmps));
        display.printPGM(TXT_MM);
      } else if( alerte != NIV_ALERTE_HAUT){
        display.printPGM(TXT_EXCLAMATION);
        display.setCursor(72, 8);
        display.printPGM(TXT_CAPACITE);
        display.setCursor(72, 16);
        display.printPGM(TXT_MAX_840L);
      } else {
        display.printPGM(TXT_EXCLAMATION);
        display.setCursor(72, 8);
        display.printPGM(TXT_TROP_PLEIN);
        display.setCursor(72, 16);
        display.printPGM(TXT_EXCLAMATION);
      }

      display.display();

      if( addr[0] != 0 && nivVolume > 700 ){
        if( date - dateTemp >= 58500 ){
          demandeMesureTemp(addr);
        }
        if( date - dateTemp > 60000){ // actualisation température toutes les minutes
          dateTemp=date;
          display.fillRect(0, 32, 128, 32, BLACK);
          display.setCursor(0,32);
          display.setTextSize(3);
          signed int temp=litTemperature(addr);
          display.printTemp16e(temp & 0xFFF0);


          if( interrupteur.active() ){ // on demande la chauffe
            display.setCursor(72, 32);
            display.setTextSize(1);
            display.printPGM(TXT_CONSIGNE);
            display.setTextSize(2);
            display.setCursor(90,40);
            display.printTemp16e(tempConsigne);
            if( temp < tempConsigne){
              display.drawLine(110, 53, 123, 40, WHITE);
              display.drawLine(123, 40, 119, 40, WHITE);
              display.drawLine(123, 40, 123, 44, WHITE);
              digitalWrite(broche_chauffe, HIGH);
            } else digitalWrite(broche_chauffe, LOW);
          } else { // pas de chauffe
            digitalWrite(broche_chauffe, LOW);
          }
        }
      }
    }

    if( !alarmeConnue || alerte==ALERTE_HAUT){
      switch(alerte){
        case ALERTE_BAS:
          if( etatAlarme && date-dateAlarme > ALERTE_BAS_PERIODE_ON){
            noTone(broche_buzzer);
            etatAlarme=false;
            dateAlarme=date;
          } else if( !etatAlarme && date-dateAlarme > ALERTE_BAS_PERIODE_OFF ){
            dateAlarme=date;
            etatAlarme=true;
            tone(broche_buzzer,ALERTE_BAS_FREQUENCE,ALERTE_BAS_PERIODE_ON);
          }
          break;
        case ALERTE_MOYEN:
          if( etatAlarme && date-dateAlarme > ALERTE_MOYEN_PERIODE_ON){
            noTone(broche_buzzer);
            etatAlarme=false;
            dateAlarme=date;
          } else if( !etatAlarme && date-dateAlarme > ALERTE_MOYEN_PERIODE_OFF ){
            dateAlarme=date;
            etatAlarme=true;
            tone(broche_buzzer,ALERTE_BAS_FREQUENCE,ALERTE_MOYEN_PERIODE_ON);
          }
          break;
        case ALERTE_HAUT:
          if( etatAlarme && date-dateAlarme > ALERTE_HAUT_PERIODE_ON){
            noTone(broche_buzzer);
            etatAlarme=false;
            dateAlarme=date;
          } else if( !etatAlarme && date-dateAlarme > ALERTE_HAUT_PERIODE_OFF ){
            dateAlarme=date;
            etatAlarme=true;
            tone(broche_buzzer,ALERTE_BAS_FREQUENCE,ALERTE_HAUT_PERIODE_ON);
          }
          break;
      }
    }
  }
}
