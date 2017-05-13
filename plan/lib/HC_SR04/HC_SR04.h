#ifndef __HC_SR04_H__
#define __HC_SR04_H__

#include <Arduino.h>

const unsigned char nbMaxMesure0 = 50;

/* Constantes pour le timeout */
const unsigned long MEASURE_TIMEOUT = 12500UL; // 12,5ms = ~2m à 340m/s

/* Vitesse du son dans l'air en m/s (ou mm/ms) */
const long SOUND_SPEED = 340;

class HC_SR04{
public:
  HC_SR04(unsigned char trigger,unsigned char echo);
  void begin();
  unsigned int mesureTmps(bool ignoreZero=true); // renvoi le temps mesuré en microsecondes
  unsigned int mesureTmpsMoyenne(unsigned char n, bool ignoreZero=true); // renvoi le temps mesuré en microsecondes
  unsigned int mesure(bool ignoreZero=true); // mesure de distance en mm
  unsigned int mesureMoyenne(unsigned char n,bool ignoreZero=true);

  unsigned int calcDist(unsigned int mesureTmps);
protected:
  unsigned char trigger_pin;
  unsigned char echo_pin;
};

#endif
