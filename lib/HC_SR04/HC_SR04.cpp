#include "HC_SR04.h"

HC_SR04::HC_SR04(unsigned char trigger,unsigned char echo){
  this->trigger_pin=trigger;
  this->echo_pin=echo;
}

void HC_SR04::begin(){
  /* Initialise les broches */
  pinMode(this->trigger_pin, OUTPUT);
  digitalWrite(this->trigger_pin, LOW); // La broche TRIGGER doit être à LOW au repos
  pinMode(this->echo_pin, INPUT);
}

unsigned int HC_SR04::mesure(bool ignoreZero){
  /* 3. Calcul la distance à partir du temps mesuré */
  return (unsigned int) ((mesureTmps(ignoreZero) * SOUND_SPEED) / 2000);
}

unsigned int HC_SR04::calcDist(unsigned int mesureTmps){
  return ((mesureTmps*SOUND_SPEED)/2000);
}

unsigned int HC_SR04::mesureTmps(bool ignoreZero){
  unsigned char i=0;
  long measure;
  do{
    if(i!=0) delay(60);
    ++i;
    /* 1. Lance une mesure de distance en envoyant une impulsion HIGH de 10µs sur la broche TRIGGER */
    delayMicroseconds(2);
    digitalWrite(this->trigger_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(this->trigger_pin, LOW);

    /* 2. Mesure le temps entre l'envoi de l'impulsion ultrasonique et son écho (si il existe) */
    measure = pulseIn(this->echo_pin, HIGH, MEASURE_TIMEOUT); // temps de l'impulsion en microsecondes
  } while( measure==0 && ignoreZero && i < nbMaxMesure0);

  /* 3. Calcul la distance à partir du temps mesuré */
  return (unsigned int) (measure);
}

unsigned int HC_SR04::mesureTmpsMoyenne(unsigned char n,bool ignoreZero){
  long measure=0;
  long m;
  unsigned char j=0;
  for(unsigned char i=0;i<n;++i){
    /* 1. Lance une mesure de distance en envoyant une impulsion HIGH de 10µs sur la broche TRIGGER */


    /* 2. Mesure le temps entre l'envoi de l'impulsion ultrasonique et son écho (si il existe) */
    m=(long)mesureTmps(ignoreZero); // temps de l'impulsion en microsecondes
    measure += m;
    if( m != 0 ) ++j;
    delay(60);
  }

  /* 3. Calcul la distance à partir du temps mesuré */
  return (unsigned int) (measure / j );
}

unsigned int HC_SR04::mesureMoyenne(unsigned char n,bool ignoreZero){

  /* 3. Calcul la distance à partir du temps mesuré */
  return (unsigned int) ((mesureTmpsMoyenne(n, ignoreZero) * SOUND_SPEED) / 2000);
}
